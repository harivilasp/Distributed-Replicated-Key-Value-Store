mat = ['A', 'B', '_'
       'B', '_', '_'
       '_', '_', 'A']

rows = len(mat)
cols = len(mat[0])
m1 = [[0 for i in range(cols)] for j in range(rows)]
m2 = [[0 for i in range(cols)] for j in range(rows)]
cnt = 0
for i in range(rows):
    for j in range(cols):
        if mat[i][j] == 'B':
            m2[i][j] = 1
        if mat[i][j] == 'A':
            m1[i][j] = 1
        if i > 0:
            m1[i][j] += m1[i-1][j]
            m2[i][j] += m2[i-1][j]
        if j > 0:
            m1[i][j] += m1[i][j-1]
            m2[i][j] += m2[i][j-1]
        if i > 0 and j > 0:
            m1[i][j] -= m1[i-1][j-1]
            m2[i][j] -= m2[i-1][j-1]

        if m1[i][j] == m2[i][j]:
            cnt += 1
            

print(cnt)
