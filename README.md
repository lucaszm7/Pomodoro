# Pomodoro

![reset](https://user-images.githubusercontent.com/42661760/214109480-951fc2d4-aeec-4ff1-90f6-ea3f75952754.png)

![studying](https://user-images.githubusercontent.com/42661760/214109502-662bf261-0452-46db-b5b7-111e9793e8c6.png)

![command_line](https://user-images.githubusercontent.com/42661760/214109639-757559c1-3d1b-4b88-a0ba-b19a0d3ca965.png)


## Build: Linux
 1. Instale as dependências
   ```
   make install
   ```
 2. Compile o Programa 
   ```
   make linux
   ```
 3. Rode :)
   ```
   make run
   ```

## Build: Windows
  ### VC++ (Developer Command Prompt VS 2022)
  1. Build & Run
  ```
  make
  ```
  ### MSYS2
  1. Remova o compilador MinGW instalado
  2. Instale o MSYS2
  ```
    winget install MSYS2.MSYS2
  ```
  3. Atualize o MSYS2, abra o terminal do MSYS2 e digite
  ```
  pacman -Syu
  ```
  4. Instale o MinGW pelo MSYS2
  ```
  pacman -S mingw-w64-x86_64-gcc
  ```
  5. Instale o Make (Caso ainda não tenha instalado)
  ```
  winget install GnuWin32.Make
  ```
  6. Adicione o Make ao PATH, o caminho é `C:\Program Files (x86)\GnuWin32\bin`
  7. Adicione o MinGW ao PATH, o caminho padrão é `C:\msys64\mingw64\bin`
  8. Compile o programa
  ```
  make windows
  ```
  9. Rode o programe
  ```
  make run_windows
  ```

## Keyboard

SPACE -> PAUSE/CONTINUE <br>
TAB   -> COMMAND LINE

