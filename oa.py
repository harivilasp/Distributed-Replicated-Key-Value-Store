import random

#Approach 1: Expand original array and try sliding window on every index (O(N * max(A[i]))
def brute(arr, k):
    A = []
    for x in arr:
        for i in range(1, x + 1):
            A.append(i)
    ans = cur = j = 0
    for i in range(len(A)):
        cur += A[i]
        if i - j + 1 > k:
            cur -= A[j]
            j += 1
        ans = max(ans, cur)
    return ans

#Approach 2: Prefix Sum + Binary search to find window's end (O(N * log N))
#Observations: 
#1. Score of each full sprint is A[i] * (A[i] + 1) / 2
#2. It's best to select later half of unfinished sprint than first half. [unfinished, finished, finished] > [finished, finished, unfinished]. We reverse the array to try all possible starts.
# For each index find the next windows end using binary seach on prefix sum array and calculate score

def solve(arr, k):
    A = arr[::-1]
    psa = [0]
    psa_score = [0]
    def cal(x):
        return x * (x + 1) // 2
    
    for x in A:
        psa.append(psa[-1] + x)
        psa_score.append(psa_score[-1] + cal(x))

    ans = 0
    start = 0
    n = len(A)

    for i in range(len(A)):
        lo, hi = i, n - 1
        while lo < hi:
            mid = (lo + hi + 1) // 2
            cnt = 0
            if psa[mid] - psa[i] < k:
                lo = mid
            else: 
                hi = mid - 1
        j = lo
        s1 = psa[j] - psa[i]
        rem = max(0, k - s1)
        p1 = psa_score[j] - psa_score[i] #score sum of full sprints
        p2 = cal(A[j]) - cal(A[j] - rem) if j < n else 0 #score of left over sprint at the back
        #print(i, j, s1, rem, p1, p2)
        cur = p1 + p2
        ans = max(ans, cur)
    return ans

#Checking if bruteforce solution equals solution2 for every possible k in a random array
# arr = [7, 4, 3, 7, 2]
# k = 3
array_size = random.randint(1, 10)
random_array = [random.randint(0, 100) for _ in range(array_size)]
arr = random_array

for k in range(1, sum(arr) + 1):
    assert(brute(arr, k) == solve(arr, k))
    print(brute(arr, k), solve(arr, k))
