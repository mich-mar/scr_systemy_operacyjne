#!/bin/bash

echo_function(){
while true;
  do echo "Strumien z procesu: $$"
  sleep 1
done
}

# "cat" - odbiera i przekazuje dane dalej, synchronizując pracę całego potoku
echo_function | cat | cat

 1) Wynik:
 michal      8432    7870  0 16:53 pts/1    00:00:00 /bin/bash ./skrypt_04.sh
 michal      8433    8432  0 16:53 pts/1    00:00:00 /bin/bash ./skrypt_04.sh
 michal      8434    8432  0 16:53 pts/1    00:00:00 cat
 michal      8435    8432  0 16:53 pts/1    00:00:00 cat

 2) Odpowiedź:
 Proces z ID=8432 uruchamia "skrypt_04.sh" i jest nadrzędnym procesem dla trzech procesów potomnych.
 Proces z ID=8433 uruchamia prawdopodobnie funkcję "echo_funcition" w skrypcie.
 Procesy z ID=8434 i ID=8435 są to procesy "cat", przekacujące dalej dane w potoku.
