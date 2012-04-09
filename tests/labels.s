jmp start
    :data1      dat 0,0,0,0
    :data2      dat 0,0,0,0
    :data3      dat 0x0000
:start
# test forward reference
	SET I, data4
	SET I, [data4]
	SET I, [x,data4]
	SET I, [x+data4]
	SET I, [data4,x]
	SET I, [data4+x]

# test backwards reference
    SET [data2], 0x4000
    SET I, [data2]
    SET [data2], I
    SET [data2], 0x4000
    SET I, data2
    SET I, [data2]
    SET I, [data2,x]
    SET I, [data2+x]
    SET I, [x+data2]
    SET I, [x,data2]

# forward reference
:data4
	data		0x99	
