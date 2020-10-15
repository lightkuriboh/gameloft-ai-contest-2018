del /s log.txt
echo Gamemode=NO_GUI>>config.txt
battle.exe Game.exe BotDemo.exe BotDemoB.exe 10
REM battle.exe Game.exe BotDemo.exe BotDemoB.exe 100
REM battle.exe Game.exe BotDemo.exe BotDemoB.exe 100
del /s config.txt