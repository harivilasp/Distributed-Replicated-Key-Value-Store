def rangeBitwiseAnd(left: int, right: int) -> int:
    ans = left 
    for i in range(left + 1, right+1, 1):
        ans = ans & i
    return ans

print(rangeBitwiseAnd(5, 7))
print(rangeBitwiseAnd(5, 7))
print(rangeBitwiseAnd(5, 7))
print(rangeBitwiseAnd(5, 7))