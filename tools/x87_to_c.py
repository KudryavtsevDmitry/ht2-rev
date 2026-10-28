"""Translate a naked inline-__asm procedure (as tools/asm_inline.py, removed after commit 986f2e5, wrote them) into C++.

    py tools\\x87_to_c.py SRC.cpp ENTRYLABEL CNAME OUT.cpp

Each instruction becomes one C statement: the general registers are DWORD
variables, the stack frame is a byte array (callees get pointers into it as
before), the x87 register stack is a set of double variables whose slots are
resolved here from the static stack depth, and jumps are gotos. With the FPU
precision control at 53 or 24 bits (all Windows uses), every x87 result is
exactly representable as a double, so storing it in a double variable keeps
the original's bits and the C operations round as the original instructions.

Callees are called with their declared conventions (the extern "C"
declarations above the naked function): fastcall takes ECX, EDX and pops its
stack arguments, cdecl gets CDECL_ARGS dwords from the stack top. The
ones that return a float in ST(0) are listed in FLOAT_RET.
"""
import re
import sys

FLOAT_RET = {'Vec3_Length', 'Trailer_SaddleLoad', 'Body_Get9508', 'Angle_F2Pi', 'Game_FrameTime', 'Copter_SideDrift'}
# the old runtime's x87 helpers: arguments and result on the x87 stack (_ftol: result in EDX:EAX)
X87_HELPERS = {'_CIpow': -1, '_CIacos': 0, '_ftol': -1}
# a cdecl callee's arguments can be pushed before an earlier call (the compiler interleaves them), so every
# cdecl call passes this many stack dwords; the extra ones are ignored and the caller pops them as before
CDECL_ARGS = 18

src, entry, cname, out = sys.argv[1:5]
cover = '--cov' in sys.argv[5:]   # count both directions of every conditional jump (g_cov, for test harnesses)
ncov = 0
text = open(src, encoding='utf-8').read().split('\n')

# the naked function holding ENTRY and the declarations just above it
start = next(i for i, l in enumerate(text) if l.strip() == entry + ':')
fn = max(i for i in range(start) if '__declspec(naked)' in text[i])
decl0 = fn
while decl0 > 0 and text[decl0 - 1].startswith('extern "C"'):
    decl0 -= 1
decls = [d for d in text[decl0:fn] if 'unsigned char King' not in d]
end = start
while text[end].strip() != '}':
    end += 1
body = [l.strip() for l in text[start:end] if l.strip() and l.strip() != 'nop' and not l.strip().startswith('_emit')]

sig = {}   # name -> (conv, nstackargs)
nargs = {}   # name -> declared argument count (the fastcall decoration)
for d in decls:
    m = re.match(r'extern "C" \w+ (__cdecl|__fastcall) (\w+)\((.*)\);', d)
    if m:
        args = [a for a in m.group(3).split(',') if a.strip()]
        sig[m.group(2)] = (m.group(1), max(0, len(args) - 2) if m.group(1) == "__fastcall" else None)
        nargs[m.group(2)] = len(args)

REG32 = ['EAX', 'EBX', 'ECX', 'EDX', 'ESI', 'EDI', 'EBP', 'ESP']
REG8 = {'AL': ('eax', 0), 'AH': ('eax', 8), 'BL': ('ebx', 0), 'CL': ('ecx', 0), 'DL': ('edx', 0)}
SIZE = {'DWORD': 'DWORD', 'WORD': 'WORD', 'BYTE': 'BYTE', 'QWORD': 'double'}


def split_ops(s):
    ops, depth, cur = [], 0, ''
    for ch in s:
        if ch == '[':
            depth += 1
        elif ch == ']':
            depth -= 1
        if ch == ',' and depth == 0:
            ops.append(cur.strip())
            cur = ''
        else:
            cur += ch
    if cur.strip():
        ops.append(cur.strip())
    return ops


def addr(expr):
    terms = []
    for t in re.split(r'\+', expr.replace(' ', '')):
        if t in REG32:
            terms.append(t.lower())
        elif re.fullmatch(r'[A-Z]{3}\*\d', t):
            terms.append('%s*%s' % (t[:3].lower(), t[4:]))
        elif t in ('KingBss', 'KingData', 'KingRdata'):
            terms.append('(DWORD)' + t)
        elif re.fullmatch(r'-?\d+', t):
            terms.append(t)
        else:
            raise SystemExit('address term ' + t)
    return '(' + '+'.join(terms) + ')'


def mem(op):
    m = re.fullmatch(r'(DWORD|WORD|BYTE|QWORD) PTR \[(.*)\]', op)
    if not m:
        return None
    return SIZE[m.group(1)], addr(m.group(2))


def ival(op):
    """C expression for a 32-bit (or 8-bit) integer operand"""
    if op in REG32:
        return op.lower()
    if op in REG8:
        r, sh = REG8[op]
        return '((%s >> %d) & 0xFF)' % (r, sh)
    m = mem(op)
    if m:
        return '*(%s*)%s' % m
    if op.startswith('OFFSET '):
        return addr(op[7:])
    if re.fullmatch(r'-?\d+', op):
        return '(DWORD)%s' % op
    raise SystemExit('operand ' + op)


def lval(op):
    if op in REG32:
        return op.lower()
    m = mem(op)
    if m:
        return '*(%s*)%s' % m
    raise SystemExit('lvalue ' + op)


def sti(op):
    m = re.fullmatch(r'ST\((\d)\)', op)
    return int(m.group(1)) if m else None


# --- static x87 depth at each instruction (worklist over the control flow)
labels = {l[:-1]: i for i, l in enumerate(body) if l.endswith(':')}
DELTA = {'fld': 1, 'fild': 1, 'fstp': -1, 'faddp': -1, 'fsubp': -1, 'fmulp': -1, 'fdivp': -1, 'fcomp': -1,
         'fcompp': -2, 'fpatan': -1}


def succ(i):
    l = body[i]
    mn = l.split()[0]
    if mn == 'jmp':
        return [labels[l.split()[1]]]
    if mn.startswith('j'):
        return [labels[l.split()[1]], i + 1]
    if mn == 'ret':
        return []
    return [i + 1]


depth = {0: 0}
work = [0]
while work:
    i = work.pop()
    d = depth[i]
    l = body[i]
    mn = l.split()[0]
    nd = d + DELTA.get(mn, 0)
    if mn == 'call' and l.split()[1] in FLOAT_RET:
        nd = d + 1
    if mn == 'call' and l.split()[1] in X87_HELPERS:
        nd = d + X87_HELPERS[l.split()[1]]
    for s in succ(i):
        if s in depth:
            if depth[s] != nd:
                raise SystemExit('x87 depth mismatch at %d: %s' % (s, body[s]))
        else:
            depth[s] = nd
            work.append(s)


def f(d, k=0):
    return 'f%d' % (d - 1 - k)


def flags_logic(r):
    return 'zf = (%s) == 0; sf = (int)(%s) < 0; cf = 0; of = 0;' % (r, r)


lines = []
maxdepth = max(depth.values()) + 1
for i, l in enumerate(body):
    if l.endswith(':'):
        lines.append('%s:;' % l[:-1])
        continue
    if i not in depth:
        raise SystemExit('unreachable: ' + l)
    d = depth[i]
    parts = l.split(None, 1)
    mn = parts[0]
    ops = split_ops(parts[1]) if len(parts) > 1 else []
    c = None
    if mn == 'mov':
        if ops[0] in REG8:
            raise SystemExit('mov to 8-bit ' + l)
        c = '%s = %s;' % (lval(ops[0]), ival(ops[1]))
    elif mn == 'lea':
        c = '%s = %s;' % (ops[0].lower(), mem(ops[1])[1])
    elif mn == 'push':
        c = '{ DWORD v = %s; esp -= 4; *(DWORD*)esp = v; }' % ival(ops[0])
    elif mn == 'pop':
        c = '%s = *(DWORD*)esp; esp += 4;' % ops[0].lower()
    elif mn in ('add', 'sub', 'xor', 'or', 'and'):
        a, b = lval(ops[0]), ival(ops[1])
        op = {'add': '+', 'sub': '-', 'xor': '^', 'or': '|', 'and': '&'}[mn]
        if ops[0] == 'ESP':
            c = 'esp %s= %s;' % (op, b)
        elif mn == 'add':
            c = '{ DWORD x = %s, y = %s, r = x + y; %s = r; zf = r == 0; sf = (int)r < 0; cf = r < x; of = (int)((x ^ r) & (y ^ r)) < 0; }' % (a, b, a)
        elif mn == 'sub':
            c = '{ DWORD x = %s, y = %s, r = x - y; %s = r; zf = r == 0; sf = (int)r < 0; cf = x < y; of = (int)((x ^ y) & (x ^ r)) < 0; }' % (a, b, a)
        else:
            c = '%s = %s %s %s; %s' % (a, a, op, b, flags_logic(a))
    elif mn == 'shl':
        c = '%s <<= %s; zf = %s == 0; sf = (int)%s < 0;' % (lval(ops[0]), ops[1], lval(ops[0]), lval(ops[0]))
    elif mn == 'inc':
        a = lval(ops[0])
        c = '{ DWORD r = %s + 1; of = r == 0x80000000; %s = r; zf = r == 0; sf = (int)r < 0; }' % (a, a)
    elif mn == 'dec':
        a = lval(ops[0])
        c = '{ DWORD r = %s - 1; of = r == 0x7FFFFFFF; %s = r; zf = r == 0; sf = (int)r < 0; }' % (a, a)
    elif mn == 'neg':
        a = lval(ops[0])
        c = '{ DWORD x = %s; cf = x != 0; of = x == 0x80000000; %s = 0 - x; zf = %s == 0; sf = (int)%s < 0; }' % (a, a, a, a)
    elif mn == 'sbb':
        a = lval(ops[0])
        c = '{ DWORD x = %s, y = %s, r = x - y - (DWORD)cf; cf = (unsigned long long)x < (unsigned long long)y + cf; %s = r; zf = r == 0; sf = (int)r < 0; of = (int)((x ^ y) & (x ^ r)) < 0; }' % (a, ival(ops[1]), a)
    elif mn == 'cmp':
        c = '{ DWORD x = %s, y = %s, r = x - y; zf = r == 0; sf = (int)r < 0; cf = x < y; of = (int)((x ^ y) & (x ^ r)) < 0; }' % (ival(ops[0]), ival(ops[1]))
    elif mn == 'test':
        c = '{ DWORD r = %s & %s; %s }' % (ival(ops[0]), ival(ops[1]), flags_logic('r'))
    elif mn in ('sete', 'setne'):
        r, sh = REG8[ops[0]]
        cond = 'zf' if mn == 'sete' else '!zf'
        c = '%s = (%s & ~0xFFu) | (%s ? 1u : 0u);' % (r, r, cond)
    elif mn == 'rep':
        assert parts[1] == 'movsd'
        c = 'memcpy((void*)edi, (void*)esi, ecx * 4); esi += ecx * 4; edi += ecx * 4; ecx = 0;'
    elif mn in ('jmp', 'je', 'jne', 'jg', 'jle', 'jl', 'jge'):
        cond = {'jmp': None, 'je': 'zf', 'jne': '!zf', 'jg': '!zf && sf == of', 'jle': 'zf || sf != of',
                'jl': 'sf != of', 'jge': 'sf == of'}[mn]
        if cond is not None and cover:
            cond = 'x87c_cov(%d, %s)' % (ncov, cond)
            ncov += 1
        c = ('goto %s;' if cond is None else 'if (' + cond + ') goto %s;') % ops[0]
    elif mn == 'ret':
        c = 'return eax;'
    elif mn == 'call' and ops[0] in X87_HELPERS:
        if ops[0] == '_CIpow':
            c = '%s = x87_pow(%s, %s);' % (f(d, 1), f(d, 1), f(d))
        elif ops[0] == '_CIacos':
            c = '%s = x87_acos(%s);' % (f(d), f(d))
        else:
            c = '{ __int64 v = (__int64)%s; eax = (DWORD)v; edx = (DWORD)(v >> 32); }' % f(d)
    elif mn == 'call':
        if ops[0].startswith('DWORD PTR'):
            target = '*(DWORD*)%s' % mem(ops[0])[1]
            conv, n = '__cdecl', CDECL_ARGS
        else:
            target = '(DWORD)x87c_' + ops[0]
            conv, n = sig[ops[0]]
            if n is None:
                n = CDECL_ARGS
        args = ['*(DWORD*)(esp+%d)' % (4 * k) for k in range(n)]
        ptypes = ['DWORD'] * n
        if conv == '__fastcall':
            args = ['ecx', 'edx'] + args
            ptypes = ['DWORD', 'DWORD'] + ptypes
        fret = ops[0] in FLOAT_RET
        rtype = 'double' if fret else 'DWORD'
        call = '((%s (%s*)(%s))%s)(%s)' % (rtype, conv, ', '.join(ptypes) or 'void', target, ', '.join(args))
        c = ('%s = %s;' % (f(d + 1), call)) if fret else ('eax = %s;' % call)
        if conv == '__fastcall' and n:
            c += ' esp += %d;' % (4 * n)
    elif mn == 'fld':
        k = sti(ops[0])
        if k is not None:
            c = '%s = %s;' % (f(d + 1), f(d, k))
        else:
            t, a = mem(ops[0])
            c = '%s = *(%s*)%s;' % (f(d + 1), 'float' if t == 'DWORD' else 'double', a)
    elif mn == 'fild':
        t, a = mem(ops[0])
        assert t == 'DWORD'
        c = '%s = *(int*)%s;' % (f(d + 1), a)
    elif mn in ('fst', 'fstp'):
        k = sti(ops[0])
        if k is not None:
            c = '' if k == 0 else '%s = %s;' % (f(d, k), f(d))
        else:
            t, a = mem(ops[0])
            c = '*(%s*)%s = (%s)%s;' % ('float' if t == 'DWORD' else 'double', a, 'float' if t == 'DWORD' else 'double', f(d))
    elif mn in ('fadd', 'fsub', 'fmul', 'fdiv', 'fsubr', 'fdivr'):
        base = mn[:4]
        op = {'fadd': '+', 'fsub': '-', 'fmul': '*', 'fdiv': '/'}[base]
        rev = mn.endswith('r')
        if len(ops) == 2:
            dst, srcop = sti(ops[0]), sti(ops[1])
            if dst != 0:   # ST(i) op= ST(0)
                assert srcop == 0
                x, y = f(d, dst), f(d)
            else:
                x, y = f(d), f(d, srcop)
            c = '%s = %s %s %s;' % (x, y, op, x) if rev else '%s = %s %s %s;' % (x, x, op, y)
        else:
            t, a = mem(ops[0])
            y = '(double)*(%s*)%s' % ('float' if t == 'DWORD' else 'double', a)
            x = f(d)
            c = '%s = %s %s %s;' % (x, y, op, x) if rev else '%s = %s %s %s;' % (x, x, op, y)
    elif mn in ('faddp', 'fsubp', 'fmulp', 'fdivp'):
        k = sti(ops[0])
        assert k >= 1 and ops[1] == 'ST(0)'
        op = {'faddp': '+', 'fsubp': '-', 'fmulp': '*', 'fdivp': '/'}[mn]
        c = '%s = %s %s %s;' % (f(d, k), f(d, k), op, f(d))
    elif mn == 'fchs':
        c = '%s = -%s;' % (f(d), f(d))
    elif mn == 'fabs':
        c = '%s = x87_fabs(%s);' % (f(d), f(d))
    elif mn == 'fsin':
        c = '%s = x87_sin(%s);' % (f(d), f(d))
    elif mn == 'fpatan':
        c = '%s = x87_atan2(%s, %s);' % (f(d, 1), f(d, 1), f(d))
    elif mn == 'fcompp':
        c = 'sw = x87_compare(%s, %s);' % (f(d), f(d, 1))
    elif mn == 'fsqrt':
        c = '%s = x87_sqrt(%s);' % (f(d), f(d))
    elif mn == 'fxch':
        k = sti(ops[0])
        c = '{ double t = %s; %s = %s; %s = t; }' % (f(d), f(d), f(d, k), f(d, k))
    elif mn in ('fcom', 'fcomp'):
        k = sti(ops[0]) if ops else 1
        if k is not None:
            y = f(d, k)
        else:
            t, a = mem(ops[0])
            y = '(double)*(%s*)%s' % ('float' if t == 'DWORD' else 'double', a)
        c = 'sw = x87_compare(%s, %s);' % (f(d), y)
    elif mn == 'fnstsw':
        c = 'eax = (eax & 0xFFFF0000u) | sw;'
    else:
        raise SystemExit('unhandled: ' + l)
    lines.append(c)

slots = ', '.join('f%d = 0' % k for k in range(maxdepth))
o = []
o.append('// %s - generated by tools/x87_to_c.py from the naked %s (%s) in %s' % (out.split('\\')[-1].split('/')[-1], entry, cname, src.replace('\\', '/')))
o.append('// Each original instruction is one statement; see the tool for why this keeps the x87 results bit for bit.')
o.append('')
o.append('#include "king.h"')
o.append('#include "x87.h"')
o.append('#include <string.h>')
o.append('')
o.append('extern "C" unsigned char KingRdata[];')
o.append('extern "C" unsigned char KingData[];')
o.append('extern "C" unsigned char KingBss[];')
o.append('')
# each callee under a private name aliased to its linker name, so a header's declaration can't clash
for name, (conv, n) in sorted(sig.items()):
    if name in X87_HELPERS:
        continue
    link = '@%s@%d' % (name, 4 * nargs[name]) if conv == '__fastcall' else '_' + name
    o.append('extern "C" void x87c_%s(); KING_ALIAS("_x87c_%s", "%s")' % (name, name, link))
o.append('')
o.append('// x87 status word bits of a compare: C0 below, C3 equal, C0|C2|C3 unordered')
o.append('static inline DWORD x87_compare(double a, double b)')
o.append('{')
o.append('    if (a < b)')
o.append('        return 0x0100;')
o.append('    if (a == b)')
o.append('        return 0x4000;')
o.append('    if (a > b)')
o.append('        return 0;')
o.append('    return 0x4500;')
o.append('}')
o.append('')
if cover:
    o.append('extern "C" unsigned g_cov[%d] = {0};' % (2 * ncov))
    o.append('extern "C" unsigned g_ncov = %d;' % (2 * ncov))
    o.append('static inline bool x87c_cov(int n, bool c) { g_cov[2 * n + c]++; return c; }')
    o.append('')
o.append('extern "C" DWORD __fastcall %s(DWORD ecx_in, DWORD edx_in)' % cname)
o.append('{')
o.append('    static_assert(sizeof(void*) == 4, "x86 only");')
o.append('    __declspec(align(16)) BYTE frame[8192];')
o.append('    DWORD eax = 0, ebx = 0, ecx = ecx_in, edx = edx_in, esi = 0, edi = 0, ebp = 0;')
o.append('    DWORD esp = (DWORD)(frame + sizeof frame - 16);   // the return address slot')
o.append('    int zf = 0, sf = 0, cf = 0, of = 0;')
o.append('    DWORD sw = 0;')
o.append('    double %s;' % slots)
targets = {l.split()[1] for l in body if l.split()[0].startswith('j')}
for c in lines:
    if c.endswith(':;'):
        if c[:-2] in targets:
            o.append(c)
    elif c:
        o.append('    ' + c)
o.append('}')
open(out, 'w', encoding='utf-8', newline='\n').write('\n'.join(o) + '\n')
print('wrote', out, len(lines), 'statements, x87 depth', maxdepth - 1)
