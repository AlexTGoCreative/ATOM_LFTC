# Definește numele fișierului executabil
$EXECUTABLE = "program.exe"

# Compilează fișierele C
gcc -o $EXECUTABLE lexer.c main.c utils.c parser.c

# Verifică dacă compilarea a avut succes
if ($?) {
    Write-Host "Compilare reușită!"
    # Rulează executabilul
    & ".\$EXECUTABLE"
} else {
    Write-Host "Compilare eșuată!"
}
