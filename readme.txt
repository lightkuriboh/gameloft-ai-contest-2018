ClientPackage ver 100

Read DAD_AI2018_GL_v100 (VN or EN) to understand about the Game

-------------------------------------------------------------------------------------------------------------
With C++

1. Go to folder BotDemoC++, and compile Bot demo (can use visual studio or any IDE that is suitable)
2. Copy file exe to ClientPackage (the folder store file StartMatch.bat), overwrite file BotDemo.exe or BotDemoB.exe
3. Run StartMatch.bat (can modify the paramater of this script) (turn per match = 500, extra = 300)


Note
- If see match runs too fast, you can open file StartMatch.bat and increase the 100 paramater (100 here is 100 miliseconds)
- Bot and Server send & receive the message through the functions : cout and getline (std out, in)
- Use std::cerr if you wnt to print the log
- Can check the log from file log.txt

------------------------------------------------------------------------------------------------------------

With Java

1. Go to folder BotDemoJava, and compile the Bot demo by command exec.bat (need to install jdk and add to system path, you can adjust the paramater in exec.bat to be suitable)
2. Copy file BotDemo.jar to ClientPackage (the folder store file StartMatch.bat), overwrite file BotDemo.jar or BotDemoB.jar
3. Run StartMatch.bat (can modify the paramater of this script)

Note:
- If see match runs too fast, you can open file StartMatch.bat and increase the 100 paramater (100 here is 100 miliseconds)
- Server send/receive the message through functions: cout và getline (std out, in)
- Bot send the message by System.out.print and receive the message by Scanner(System.in)
- Use System.err.println to print the log
- Can check the log from file log.txt
--------------------------------------------------------------------------------------------------------------
MORE INFO:
Run StartMatch_Extra.bat to debug Extra time mode quickly
Run StartMatch_Pen.bat to debug Penally mode quickly
Run StartMatch_No_GUI.bat to debug fastly with no GUI & no console log
Run StartMatch_Short.bat to debug with turn per match = 250 and extra = 150