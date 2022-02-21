# Test case documentation

> ## g. Test case documentation (10%)
> With all functionality, this is already a complex program, so it has to be tested thoroughly.
>
> Design four different test cases that not only test a single functionality (like scheduling an alarm), but also a
> combination of functions of your program.
>
> Describe each test case: what do you do to test this case, what does the test do, what is the expected result?

## Test case 1 - Scheduling and deleting alarms
1. Schedule an alarm sometime in the future.
2. Listing alarms should return 1 alarm (Alarm 1).
3. Delete alarm 2.
4. Listing alarms should return 1 alarm still, as no "Alarm 2" exists.
5. Delete alarm 1.
6. Listing alarms should not return any alarms.
7. No alarms should ring after this test.

## Test case 2 - Alarm time string formatting
1. Schedule an alarm sometime in the future (For example "2323-12-12 23:23:23").
2. Listing alarms should return 1 alarm (Alarm 1).
3. Schedule an alarm sometime in the past (For example "1212-12-12 12:12:12").
4. Listing alarms should return 1 alarm still, as an alarm in the past is not valid.
5. Schedule an alarm with a non-existing time (For example "2424-12-12 24:24:24").
6. Listing alarms should return 1 alarm still, as an alarm with a non-existing time is not valid.
7. Schedule an alarm with a random string (For example "Hello World!").
8. Listing alarms should return 1 alarm still, as is should be impossible to create an alarm with a random time string.

## Test case 3 - Correct ringing
1. Schedule an alarm sometime in the future.
2. Wait until the set time. A sound should be played.
3. Schedule an alarm sometime in the future.
4. Delete the new alarm.
5. Wait until the set time. A sound should not be played.
6. Schedule two alarms to different future time.
7. Delete the first one.
8. Wait until the first set time. It should not ring.
9. Wait unitil the second set time. It should ring.

## Test case 4 - Counting processes
1. Count the processes running this program. It should be excactly one.
2. Schedule an alarm sometime in the future.
3. Count the processes running this program. I should be excactly two.
4. Schedule an alarm sometime after the first alarm.
5. Count the processes running this program. I should be excactly three.
6. Wait for the first alarm to ring. The process count should now be two.
7. Cancel the remaining alarm. The process count should now be one.
