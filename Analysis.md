# Increase and Decrease in Brightness

## Packet Analysis

### USB URB

0000   40 ae 42 35 ff 9e ff ff 53 02 00 05 01 00 00 00   @.B5....S.......
0010   2a c2 96 66 00 00 00 00 57 9f 06 00 8d ff ff ff   *..f....W.......
0020   41 00 00 00 41 00 00 00 21 09 0a 03 01 00 41 00   A...A...!.....A.
0030   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................

1 - 6 packet common in terms of ID. 7th and 8th Apcket different with no apparent pattern(!yet)
9 URB type= Submit 53
10 URB transfer type= Control 02
11 Endpoint= 00
12 Device= 5
13 URB bus ID= 1
14 - 15 common 0
next 8 bytes are URB sec= [6 same bytes and 2 diff no pattern]
next 4 bytes are URB usec= [4 diff bytes no pattern]
next 4 bytes are URB status= [same 4 bytes]
next 8 bytes 4 each of URB and data length
others irrelevant(!for now)'


### Data

0040   0a 01 01 02 29 14 01 01 xx 00 00 ff 00 02 00 00   ....)...........
0050   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
0060   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
0070   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00   ................
0080   10                                                .

all data bytes same except xx
xx == 01 if increase else xx == 00 for decrease

After further Analysis, following are the bytes for each setting.

1. 6 = Animation cycle (0x01 -> 0x05)
2. 8 = Animation mode (0x01 -> 0x15)
3. 9 = Brightness (0x00 -> 0x05)
4. 10, 11, 12 = r, g, b values (0x00 -> 0xff)
5. 13 = Colour Mixing 
6. 14 = Sleep timer (0x00 -> 0x05)
