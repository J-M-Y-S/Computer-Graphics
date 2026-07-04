@echo off
cd /d "E:\PY\FINAL"
git add -A
git commit -m "Update %date% %time%"
git push
echo.
echo ====== Upload completed! ======
pause
