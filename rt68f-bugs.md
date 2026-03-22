# Bugs with the RT68F porting

## PS/2

### Print Screen
Expected E0 12 E0 7C / E0 F0 7C E0 F0 12

There is a bug somewhere, the actual sequence 
received is missing some codes and it's not even 
consistent. It might be a problem in the FPGA component.
Example of code sequence for print screen:
- `e0 12 7c e0 f0 e0 f0 12`
- `e0 12 7c e0 f0 04 f0 12`

