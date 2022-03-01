# TaskManager
<p>This is a library that can be used to register and execute tasks at a set frequency.

## Acknowledgments
<p>Far back in time I built the precursors of this library on some excellent code from
Simon Monk. I can't remember which book of his that I was working through at the time,
but all of his books are tremendously helpful and insightful. I highly recommend his
work. The two classes in the library, Event and Timer, are the ones I built on top of,
and all credit and copyright go to Simon Monk.</p>

## Purpose
<p>The purpose of the two original classes was to allow code to be broken into
discrete parts and then be called at a controlled, consistent rate. I decided to combine
that functionality with the ability to start and stop the execution of the code. I wrote
code to monitor a button in the circuit, and when the button was pressed it would start
or stop the code execution. This led to initializing stuff at the start of the sketch,
then initializing stuff at the start of execution, then cleaning up after execution...
and this library was formed.</p>
<p>I have personally used this library on many of my robots with a pushbutton to
control the starting and stopping of the robot, callbacks to read the various sensors
at different rates, and other callbacks to control the actions of the robot itself.
I hope that you find it useful in your own projects.</p>

## Design
As mentioned, this library can be used to register and execute tasks at a set frequency.
This is done using 'callbacks,' methods with a simple, expected interface. The callbacks
are registered with the Task Manager and a frequency of how often to call is specified.
Different callbacks can have their own frequency and state data that gets stored between
calls.</p>
<p>In addition, the Task Manager will call "lifetime" callbacks. These are special
callbacks not associated with any task, but that will be called at various points of the
sketch execution. Setup, start, stop, and idle callbacks will be called at the
appropriate times by the Task Manager.</p>
<p>The Task Manager execution can be started and stopped "manually" within the code
as needed, but it can also be setup to monitor a pin for state change. When the state
changes the Task Manager will start or stop execution. The pin can be connected to a
pushbutton, with the button controlling the starting and stopping of the execution.</p>

## Caveat
This library is something of a 'task manager', and it is built with something called
a 'timer'. However, it does not use any of the hardware timers found in Arduino. It uses
a software based, best effort timer. While it is pretty accurate when running just a few,
quickly executed tasks, don't expect it to be entirely accurate. If two tasks are scheduled
to execute at the same frequency, one is going to run before the other, in a random order.
If one task takes a long time to execute, its execution may run over the expected start
time of a subsequent task. Be aware of these limitations.

## Examples
<p>The example sketches demonstrate almost all of the Task Manager features
within various scenarios.</p>

### start_stop_button
<p>This sketch demonstrates the original purpose of this library, starting and stopping
the execution of the sketch based on a push button. An example circuit diagram that works
with the code is provided. Some assembly required.</p>

### immediate_start
<p>This sketch demonstrates the use of the Task Manager library to schedule some callbacks
and then begin execution. This is useful if all you want to do is call code at regular
intervals with no need to control the starting/stopping of the execution.</p>

### start_stop_manual
<p>This sketch builds on immediate_start and demonstrates the sketch itself 'manually'
stopping and restarting execution using the Task Manager.</p>

### dynamic_callback
<p>This sketch builds even further, demonstrating callbacks being dynamically added and
removed while the Task Manager is executing.</p>

