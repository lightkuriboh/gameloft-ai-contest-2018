del /s log.txt
if exist config.txt (
	del /s config.txt
)
echo Gamemode=Short>>config.txt
battle.exe Game.exe BotDemo.exe BotDemoB.jar 100
REM battle.exe Game.exe BotDemo.exe BotDemoB.exe 100
REM battle.exe Game.exe BotDemo.exe BotDemoB.exe 100
del /s config.txt