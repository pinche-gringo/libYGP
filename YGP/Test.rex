[a*b]
ab=1
aaaab=1
b=1
aabc=0
sucker=0

[\[u-z\]*b]
ub=1
uvwxyzb=1
b=1
uvbc=0
sucker=0

[\[\]ac\]*b]
]acb=1
]dcb=0

[\[^\]\]*b]
b=1
ab=1
]b=0

[a+b]
ab=1
aaaab=1
b=0
aabc=0

[a.b]
axb=1
aaaab=0
b=0
ab=0
abc=0

[a?b]
ab=1
aaaaab=0
b=1
aab=0
acb=0

[\[:alpha:]]*]
ab=1
aaaaab=1
a1b=0

[ab\\>.*\\<cd]
ab cd=1
ab ff cd=1
abcd=0
cd ab=0
abcd=0

[\\>cd]
cd=0

[ab\\<]
ab=0

[ab\\>.*]
ab cd=1
cd=0

[\\<cd]
cd=1
acd=0

[ab\\b.\\bcd]
ab.cd=1
acd=0

[ab\\Bcd]
abcd=1
ab cd=0