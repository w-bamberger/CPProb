@ECHO OFF
set TEXTTEST_HOME=%~dp0
for /F %%i IN ("%TEXTTEST_HOME%\..\build-test\Debug\cpprobtest.exe") DO set test_executable=%%~fi

echo.
echo TEXTTEST_HOME:   %TEXTTEST_HOME%
echo test_executable: %test_executable%
echo.
C:\Texttest\texttest-3.23\source\bin\texttest.py
