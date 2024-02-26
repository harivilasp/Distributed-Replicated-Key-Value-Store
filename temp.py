class Solution:
    def earliestSecondToMarkIndices(self, nums: List[int], changeIndices: List[int]) -> int:
        su = sum(nums)
        n = len(nums)
        cnt = [0]*n
        ind = -1
        ccnt = n
        for i in range(len(changeIndices)):
            if cnt[changeIndices[i]-1] == 0:
                cnt[changeIndices[i]-1] = 1
                ccnt -= 1
            if ccnt == 0:
                break
        
        if i <= len(changeIndices):
            if i >= n + su:
                return i + 1
            if n + su < len(changeIndices):
                return n + su
        return -1
    
        
            