Oto poprawiona wersja wyjaśnienia dotyczącego funkcji Pthread:

---

### 1. **`pthread_create`**:

Ta funkcja tworzy nowy wątek i uruchamia go, wykonując funkcję, którą jej przekażesz. Wątek to jednostka wykonawcza,
która działa równolegle z innymi wątkami. `pthread_create` wymaga przekazania funkcji, która będzie uruchomiona w nowym
wątku oraz odpowiednich argumentów, które mogą zostać przekazane do tej funkcji.

**Przykład:**

```c
pthread_create(&thread_id, NULL, thread_function, NULL);
```

Tworzy wątek, który uruchomi funkcję `thread_function`.

---

### 2. **`pthread_join`**:

Ta funkcja powoduje, że wątek wywołujący (np. wątek główny) czeka na zakończenie wątku, na którym została wywołana.
Jeśli wywołasz `pthread_join` na wątku, program zatrzyma się, aż ten wątek zakończy swoje działanie. Jest to przydatne,
gdy chcesz upewnić się, że wątki zakończyły swoje zadania przed kontynuowaniem dalszego działania programu.

**Przykład:**

```c
pthread_join(thread_id, NULL);
```

Wątek główny czeka, aż wątek `thread_id` zakończy swoje działanie.

---

### 3. **`pthread_exit`**:

Ta funkcja kończy działanie bieżącego wątku. Wątek, który wywoła tę funkcję, zakończy swoje działanie i zwróci określoną
wartość. Jeśli jest to wątek główny, funkcja powoduje zakończenie całego programu, ale tylko po tym, jak inne wątki
zakończą swoją pracę (o ile zostały poprawnie połączone).

**Przykład:**

```c
pthread_exit(NULL);
```

Zakończenie wątku i przekazanie wartości `NULL` jako kodu zakończenia.

---

### 4. **`pthread_mutex_init`**:

Ta funkcja inicjalizuje obiekt muteksu, który jest używany do synchronizacji wątków, zapobiegając równoczesnemu
dostępowi do wspólnych zasobów. Muteks (ang. **mutual exclusion**) jest mechanizmem zapewniającym, że tylko jeden wątek
w danym momencie może wykonać określoną część kodu, co zapobiega wystąpieniu niepożądanych efektów ubocznych w przypadku
dostępu do wspólnych danych.

**Przykład:**

```c
pthread_mutex_init(&mutex, NULL);
```

Inicjalizuje muteks o nazwie `mutex`.

---

### 5. **`pthread_mutex_lock`** i **`pthread_mutex_unlock`**:

Te funkcje służą do blokowania i odblokowywania muteksu. Kiedy wątek wywołuje `pthread_mutex_lock`, wątek zyskuje "
zamek" na dany zasób, co oznacza, że żaden inny wątek nie może uzyskać do niego dostępu, dopóki wątek nie wywoła
`pthread_mutex_unlock`, który zwolni blokadę.

**Przykład:**

```c
pthread_mutex_lock(&mutex);
// kod chroniony przez muteks
pthread_mutex_unlock(&mutex);
```

Funkcja `pthread_mutex_lock` blokuje muteks, a `pthread_mutex_unlock` go odblokowuje, pozwalając innym wątkom na dostęp.

