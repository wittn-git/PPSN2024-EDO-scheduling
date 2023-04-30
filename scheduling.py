m = 1                       # number of machines
n = 3                       # number of jobs
p = [1,2,3]                 # processing times                 
r = [0,0,3]                 # release times
d = [1,3,4]                 # due dates

# Calculation of starting times for each job given an a set of orderings X
def S(X, m, n, p, r, d):
    s = [0] * n
    for i,x in enumerate(X):
        if i == 0:  s[x] = max(0, r[x])
        else:       s[x] = max(s[X[i-1]]+p[X[i-1]], r[x])
    return s

# Calculation of completion times for each job given an a set of orderings X
def C(X, m, n, p, r, d):
    c = S(X, m, n, p, r, d)
    for i in range(n):
        c[i] += p[i]
    return c

# Calculation of tardiness for each job given an a set of orderings X
def T(X, m, n, p, r, d):
    t = C(X, m, n, p, r, d)
    for i in range(n):
        t[i] -= max(0, d[i])
    return t

# Calculation of in-time jobs given an a set of orderings X
def U(X, m, n, p, r, d):
    t = T(X, m, n, p, r, d)
    return [1 if t[i] <= 0 else 0 for i in range(n)]

print("S(X) = ", S([0,2,1], m, n, p, r, d))
print("C(X) = ", C([0,2,1], m, n, p, r, d))
print("T(X) = ", T([0,2,1], m, n, p, r, d))
print("U(X) = ", U([0,2,1], m, n, p, r, d))