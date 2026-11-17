@echo off
REM Publica este projeto em https://github.com/bonutt/gota-inteligente-iot
REM Uso: de dois cliques neste arquivo (precisa do Git instalado)
cd /d "%~dp0"
set LOG=publicar_log.txt
echo Publicando no GitHub... > %LOG%
if not exist ".git" git init -b main >> %LOG% 2>&1
git config user.name >nul 2>&1 || git config user.name "Luigi Bonuccelli"
git config user.email >nul 2>&1 || git config user.email "bonuccellibr@gmail.com"
git add -A >> %LOG% 2>&1
git commit -m "Kit IoT ESP32 da Gota Inteligente - entrega de Physical Computing" -m "Equipe Gota Inteligente - FIAP Global Solution 2026.2" -m "Co-authored-by: Henrique Maciel Rodrigues <rm559628@fiap.com.br>" -m "Co-authored-by: Igor Pereira Nociti <rm560225@fiap.com.br>" -m "Co-authored-by: Joao Pedro Vieira de Moais <rm560527@fiap.com.br>" -m "Co-authored-by: Luigi Bonuccelli <rm560950@fiap.com.br>" -m "Co-authored-by: Pedro Paulo Sabino Botelho <rm559578@fiap.com.br>" -m "Co-Authored-By: Claude Opus 5.5 <noreply@anthropic.com>" -m "Claude-Session: https://claude.ai/code/session_01Egwzw9iu1tVFqPdUgieuAZ" >> %LOG% 2>&1
git branch -M main >> %LOG% 2>&1
git remote remove origin >nul 2>&1
git remote add origin https://github.com/bonutt/gota-inteligente-iot.git >> %LOG% 2>&1
echo Enviando (se abrir uma janela de login do GitHub, entre com a conta bonutt)...
git push -u origin main >> %LOG% 2>&1
if errorlevel 1 (echo FALHOU >> %LOG%) else (echo SUCESSO >> %LOG%)
type %LOG%
echo.
echo Pronto. Pode fechar esta janela.
pause
