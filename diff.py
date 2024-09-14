import colorama
from colorama import Fore, Style
from difflib import SequenceMatcher
import re

# 初始化 colorama
colorama.init(autoreset=True)

def highlight_differences(line1, line2):
    # 使用正则表达式将单词和空格分离
    words1 = re.findall(r'\S+|\s+', line1)
    words2 = re.findall(r'\S+|\s+', line2)

    # 创建结果列表
    result1 = []
    result2 = []

    # 移除纯空格的元素来进行匹配
    non_space_words1 = [w for w in words1 if not w.isspace()]
    non_space_words2 = [w for w in words2 if not w.isspace()]
    words1.append("")
    words2.append("")
    # 使用 difflib.SequenceMatcher 匹配非空格部分
    matcher = SequenceMatcher(None, non_space_words1, non_space_words2)

    for tag, start1, end1, start2, end2 in matcher.get_opcodes():
        if tag == 'equal':  # 如果单词相同
            for i in range(start1, end1):
                result1.extend(non_space_words1[i]+words1[2*i+1])
            for i in range(start2, end2):
                result2.extend(non_space_words2[i]+words2[2*i+1])
        elif tag == 'replace':  # 如果单词被替换
            for i in range(start1, end1):
                result1.extend('<span style="color:red">' + non_space_words1[i] + '</span>'+words1[2*i+1])
            for i in range(start2, end2):
                result2.extend('<span style="color:red">' + non_space_words2[i] + '</span>'+words2[2*i+1])
        elif tag == 'delete':  # 如果单词被删除
            for i in range(start1, end1):
                result1.extend('<span style="color:red">' + non_space_words1[i] + '</span>'+words1[2*i+1])
        elif tag == 'insert':  # 如果单词被插入
            for i in range(start2, end2):
                result2.extend('<span style="color:red">' + non_space_words2[i] + '</span>'+words2[2*i+1])
    return ''.join(result2)


def find_line(a, b):
    for one in b:
        if re.match(rf"^{a[0:3]}", one):
            return one
    return ""

f = open("res.txt",'r')
e1 = []
e2 = []
line = f.readline()
while not re.match(r'^\d+$', line):
    line = f.readline()

print("<html>")
while 1:
    line = f.readline()
    while not re.match(r'^\d+$', line):
        e2.append(line)
        line = f.readline()
        if not line:
            break
    if e1==[]:
        e1 = e2
        continue
    mkb = '<p style="margin: 0; padding: 0; line-height: 1.5; white-space: pre;font-weight: bold;">'
    mke = "</p>"
    for one in e2:
        if find_line(one, e1):
            a = highlight_differences(find_line(one, e1), one)
            print(mkb + a[:-1] + mke)
        else:
            print(f'{mkb}<span style="color:red">' + one[:-1] + f'</span>{mke}')
    e1 = e2
    e2 = []
    if not line:
        break
print("</html>")
