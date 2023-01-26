# TaskManager
<p>This is an Arduino library that can be used to register and execute tasks at a
set frequency.

## Purpose
<p>When writing Arduino code that is more complicated than a single task, it usually
means that several things need to be checked periodically, and when when the right
time comes, something gets executed. The TaskManager library provides a simple to use
framework to write the 'something that gets executed', a Task, and to execute it at a
specified frequency, like every second.</p>
<p>The TaskManager library can be used in its simpleset form of adding the tasks and then
starting the task manager which never stops. Or it can be configured to monitor a 
momentary push button allowing you to start and stop the task manager with the push
of the button.</p>
<p>I have personally used this library on many of my robots with a pushbutton to
control the starting and stopping of the robot, callbacks to read the various sensors
at different rates, and other callbacks to control the actions of the robot itself.
I hope that you find it useful in your own projects.</p>

## History
<p>This is version 2.0 of the TaskManager library. Unfortunately, it is not a
drop-in replacement for the 1.0 version. Version 1.0 had a more complicated
interface and api using method callback pointers and such. While the concepts
are pretty much the same, the classes and interface are different. It is a
simple task (no pun intended) of moving existing code into the version 2.0 classes
and api. But you will have to do some work. I'm sorry about that. As such, the
previous use of classes from Simon Monk has been replaced and removed. I hope you
find this version to be much simpler and usable.</p>

## Design
<p>The design is very straight forward. Functionality is broken up between Tasks that
get executed, and the TaskManager that executes the tasks. Tasks are added to the
TaskManager with a rate, or period, specified in milliseconds. The period is how
often the Task will be executed. Multiple tasks can be added to the TaskManager,
and each executed at its own period. So, you could have a task that checks records
a sensor value every 100 milliseconds and another task that prints out the current
value every second.</p>
<p>Because almost every Arduino sketch uses a blinking LED to indicate it is running
normally, a BlinkTest class is provided. You can create instances of BlinkTest and
add them to the TaskManager to blink at any period you want. You can have it use the
LED_BUILTIN LED or you can specify a different pin. Blinking is so common, that the
TaskManager has one built into it so you can avoid creating a BlinkTest instance
all the time.</p>
<p>Sometimes you may want to control the execution of the sketch, controlling when it
starts, and stopping it when needed. The TaskManager can be configured to monitor
a momentary push button on a designated pin. And when the button is pressed the task
manager will be started. When it is pressed again the task manager will be stopped.</p>
<p>The TaskManager can be started and stopped via a button as described above, or
programmatically in the sketch code. In either case, there will be times with the
task manager is idle. An idle task can be added to the task manager that will executed
only when the task manager is idle, and a specified period. So, you could have an
instance of a BlinkTask as the idle task that blinks at a faster rate than a BlinkTask
you have installed and running when the task manager is running. Easy feedback for
when the task manager is executing or idle.</p>

### Task class
The Task class is a simple method with just four methods. It is not required to
implement all the methods, only the methods you need. The Task base class has
empty versions of each method so you don't have to implement your own empty versions.

#### setup()
<p>The setup method is called only once, when the task is first added to the
task manager. Actions that only need to be performed once for setup should be in
this method. For example setting up pin input/output modes, or setting variables
to their starting values.</p>

#### start()
<p>The start method is called every time the task manager is started. So, it is a 
good place to reset variables to their starting values if their state does not need
to be maintained across task manager starting and stopping.</p>
<p>Tasks can be added to the TaskManager at any time, even when it is executing. So,
the task manager takes care to call the start method when it is already executing so that
you can count on consistent behavior.</p>

#### update()
<p>The update method is called every time the task is supposed to execute. The task
manager has already determined that it is time for the task to execute according to the
period that was given when the task was added. So, the task just needs to implement its
functionality, whatever you want the task to do.</p>

#### stop()
<p>The stop method is called every time the task manager is stopped. So, it is a good
place to place any cleanup code that needs to happen. The task manager can always be
restarted, and the start method will be described as above, so don't completely cleanup
variables that will be needed.</p>

## Caveat
<p>This library provides a 'task manager' that will execute tasks at defined rates.
However, it does not use any of the hardware timers found in Arduino. It provides
a software based, best effort algorithm to determine when to execute the tasks. While it
is pretty accurate when running just a few, quickly executed tasks, don't expect it to be
entirely accurate. If two tasks are scheduled to execute at the same period, one is going
to run before the other, in a random order. If one task takes a long time to execute,
its execution may run over the expected start time of a subsequent task. Just be aware of
these limitations.</p>

## Dependencies
<p>This library has a dependency on another library I have released called ArduinoLogging 
  (https://github.com/markwomack/ArduinoLogging). It uses it to print
debug and state messages. You can either download and install the ArduinoLogging library
to the Arduino library directory, or you can change the calls to use Serial directly, or
you can comment them out completely.</p>

## Examples
<p>The example sketches demonstrate almost all of the TaskManager features
within various scenarios.</p>

### start_stop_button
<p>This sketch demonstrates the Task Manager library in starting and stopping the
execution of the sketch basedon a push button. An example circuit diagram that works with
the code is provided. Some assembly required.</p>

### immediate_start
<p>This sketch demonstrates the use of the Task Manager library to schedule some callbacks
and then begin execution. This is useful if all you want to do is call code at regular
intervals with no need to control the starting/stopping of the execution with a button.</p>

### start_stop_manual
<p>This sketch builds on immediate_start and demonstrates the sketch itself 'manually'
stopping and restarting execution using the Task Manager.</p>

### dynamic_callback
<p>This sketch builds even further, demonstrating callbacks being dynamically added and
removed while the Task Manager is executing.</p>

