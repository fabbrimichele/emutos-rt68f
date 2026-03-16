# Bugs with the RT68F porting

With time interrupt enable and VBL interrupt disabled.
Board is switched of and on for each test.

Same error 3 times in a row, I don't know if that's by
chance but it's always after `bios_init() end`

## First try
```
bios_init() end
Panic: Line F Emulator
sr=5555 pc=000d0000

D0-3: 00047620 20737973 74656d20 666f6e74
D4-7: 00000000 00000000 00000000 00000000
A0-3: 000000ff 000d000b 00080002 00020007
A4-7: 00080001 00070001 00015555 00003114
 USP: 00000000

basepage=00003404
text=00000000 data=00000000 bss=00000000
```

## Second try
```
bios_init() end
Panic: Illegal Instruction
sr=2704 pc=00000421

D0-3: 00002700 00000028 76202004 000000e2
D4-7: 000000fe 00000050 00000000 00000019
A0-3: 000405c2 00000050 00042db8 00044af0
A4-7: 00040000 00045134 0004e024 00000e50
 USP: 00000000

basepage=00003404
text=00000000 data=00000000 bss=00000000
```

## Third try
```
bios_init() end
Panic: Line F Emulator
sr=5555 pc=000d0000

D0-3: 00047620 20737973 74656d20 666f6e74
D4-7: 00000000 00000000 00000000 00000000
A0-3: 000000ff 000d000b 00080002 00020007
A4-7: 00080001 00070001 00015555 00003114
 USP: 00000000

basepage=00003404
text=00000000 data=00000000 bss=00000000
```