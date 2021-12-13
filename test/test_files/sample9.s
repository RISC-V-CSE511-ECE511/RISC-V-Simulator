lui r17 1024
add r1 r2 r3
addi r16 r16 10
sw r17 2(r16)
lw r17 2(r16)
beq r1 r1 m1
add r6 r6 r6
m1:
bne r1 r17 m2
sub r6 r6 r6
m2:
blt r1 r17 m3
xor r6 r6 r6
m3:
bge r17 r1 m4
and r6 r6 r6
m4:
xor r1 r1 r1
xor r2 r2 r2
xor r3 r3 r3
addi r2 r2 10
addi r3 r3 1
lp_start:
sub r2 r2 r3
and r6 r6 r6
beq r2 r1 lp_end
bne r2 r1 lp_start
lp_end:
xor r1 r1 r1
or r6 r6 r6
add r6 r6 r6
jal r5 subtract
beq r6 r6 end
add r6 r6 r6
subtract: 
add r1 r1 r3
jalr r4 r5 0
end:
add r6 r6 r6