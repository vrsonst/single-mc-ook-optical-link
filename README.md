# SINGLE MICRONTROLLER, ON-OFF KEYING OPTICAL LINK
Transmitter-Receiver system that works on a single microntroller. Code uses On-Off Keying to encode a message, sends it as light pulses through an LED to an LDR and then decodes it back to text.

### HOW TO USE.
- Make your circuit. Make sure LED's pin goes to a digital I/O and LDR's to an analogue, through the voltage divider. Then adjust in the code the pins you have connected (for reference, I used pin12 for the LED and A0 for the LDR).
- Use the "ldr-Vo" code to determine the LIGHT_THRESHOLD.
- Upload "led-ldr-ook" to microcontroller and edit LIGHT_THRESHOLD variable. Make sure it sits somewhere between the ON and OFF values you got from running "ldr-Vo".
- You can also play with BIT_TIME if you want a slower or faster Transmitter, just don't drop lower than what the LDR can read (with BIT_TIME = 125ms it's about 8Hz).
- Enter text via Serial Monitor (max 32 characters, unless you edit LCD variable in code) and enjoy! :D

### DEMONSTRATION

[![Demonstration video](https://img.youtube.com/vi/nTW_Ap7Q5Dg/maxresdefault.jpg)](https://www.youtube.com/watch?v=nTW_Ap7Q5Dg)

### MORE ON CODE.
Both main functions _(Transmitter and Receiver)_ have four states: Idle, Start, Data and End. The purpose of those states is to control both the transmitter and the receiver at the same time, in a single loop, thus allowing us to use a single microcontroller instead of needing two (one for each).

**Transmitter function.** The transmitter function sits in _IDLE_ state until the user has typed a message and pressed enter. It saves the message in a char array, sandwiched between two extra very helpful bytes: STX (0x02) and ETX (0x03). STX and ETX signal the start and the end of our message, making the array look like this:

```
TEXT = [0x02, H, e, l, l, o,  , w, o, r, l, d, 0x03, \0]
```

Then it switched to the _START_ state. The transmitter will then emit the first bit, called Start Bit. The purpose of this little blink is to indicate to the Receiver that bit begins (the receiver couldn't possible understand the difference between a 0 bit and, well, the message not being transmitted yet). The transmitter can switch now to the _DATA_ state, in which it starts sending the message in bits through light pulses: On when bit=1 and Off when bit=0. It reads the LSB from each byte and shifts to the right, hence making our byte look like this:

```
Letter      Start bit              Byte
  H      =      1        0  0  0  1  0  0  1  0
```

(H is "normally" 01001000). Once Data finishes sending a bit, it can move on to the _END_ state where it will either return to _Start_ if it isn't done sending the message or, if it is, it will reset and return to _Idle_.


**Receiver function.** The Receiver function bears a similar logic, it too using the four states but in a slightly different way. The _IDLE_ state for the receiver means it waits for the Start Bit to arrive, the bit indicating the start of a byte and, when that happens, it switches to the _START_ state.

There it compares what it sees (through the Analogue pin) and if it's lower than the set LIGHT_THRESHOLD, it creates a 0 while otherwise it creates an 1, slowly recreating the message, saving it in a new variable. It is important to note that the receiver tries to check the transmitter in the middle of its state.

If, for example, we had a BIT_TIME = 500ms, for our letter 'H' (100010010):

```
TRANSMITTER  Start Bit      0         0         0         1         0         0         1         0
                  |         |         |         |         |         |         |         |         |
                  0ms      500ms    1000ms    1500ms  .  .  .
RECEIVER               |         |         |
                      250ms     750ms    1250ms  .  .  .
```

The _Start_ state can finally move on to the _DATA_ one, which has the role of checking whether the message has began and finished, reacting accordingly. Once everything is finished and the Receiver has seen the ETX (End of Text) byte, it can finally move on to _END_ state, where it prints the message and switches back to _Idle_.

Non-blocking coding was used in order for these two functions to run at the same time.


### BUT... WHY?
It started as a foundation project for me to understand optical links better. Using this as a base, it will be easier to move on to bigger projects (using multiple lasers and photodiodes in the future to emulate a more complex link).

> _(Code is in desperate need of optimisation)._
> I struggled too much to make this work in a single microcontroller idk why.
