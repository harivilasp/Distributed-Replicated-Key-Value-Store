class Solution:
    def countPrefixSuffixPairs(self, words: List[str]) -> int:
                
        class Node:
            def __init__(self, l=0):
                self.chars = defaultdict(Node)
                self.l = l
                self.cnt = set()
        
        root1 = Node(1)
        root2 = Node(1)
        ans = 0
        n = len(words)
        for i in range(n-1, -1, -1):
            f1, f2 = set(), set()
            st = words[i]
            temp = root1
            ll = len(st)
            j = 0
            for ch in st:
                temp2 = temp.chars[ch]
                if j == ll - 1:
                    f1 = temp2.cnt.copy()
                temp2.l = temp.l + 1
                temp = temp2
                temp.cnt.add(i)
                j += 1
            
            # if i in f1:
            #     f1.remove(i)
            # f1.remove(i)
            # print(st, f1, temp.cnt, temp.l)
            
            st = st[::-1]
            temp = root2
            j = 0
            for ch in st:
                temp2 = temp.chars[ch]
                if j == ll - 1:
                    # f1 = temp2.cnt.copy()
                    f2 = temp2.cnt.copy()
                temp2.l = temp.l + 1
                temp = temp2
                temp.cnt.add(i)
                j += 1
            # if i in f2:
            #     f2.remove(i)
            # print(st, f2, temp.cnt, temp.l)
            
            # print(f1, f2, f1.intersection(f2))
            # if f1 and f2:
            ans += len(f1.intersection(f2))
        
        return ans
