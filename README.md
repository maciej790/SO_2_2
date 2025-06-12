### Wielowątkowy Serwer Czatu
Autorzy:
# Wielowątkowy Serwer Czatu
### Autorzy:

Dominik Filipiak (273479)
### Dominik Filipiak (273479)

Kacper Kostrzewa (272855)
### Kacper Kostrzewa (272855)

# 📝 Opis projektu
Projekt realizuje prosty, wielowątkowy system czatu działający w architekturze klient-serwer.

# 🔧Części składowe:
Serwer czatu – nasłuchuje połączeń od klientów, obsługuje wielu użytkowników jednocześnie i rozsyła wiadomości do wszystkich uczestników.

Klient czatu – łączy się z serwerem, umożliwia wysyłanie i odbieranie wiadomości od innych użytkowników.

# 🛠 Wymagania
System operacyjny: Windows

Kompilator: C++ (obsługujący standard C++11 lub nowszy)

Biblioteka: ws2_32.lib

# 🚀 Kompilacja i uruchomienie
Serwer
```sh
sh
g++ server.cpp -o server.exe -lws2_32
.\server.exe
```
Klient
```sh
sh
g++ user.cpp -o user.exe -lws2_32
.\user.exe
```
# 🏗 Struktura programu
### Serwer
Inicjalizacja WinSock – przygotowanie do komunikacji sieciowej.

Utworzenie gniazda TCP – nasłuchiwanie na porcie 8080.

Akceptowanie klientów – dla każdego nowego połączenia tworzony jest osobny wątek.

### Obsługa użytkownika:

Wprowadzenie nicku.

Dodanie użytkownika do globalnej listy.

Odbieranie i rozsyłanie wiadomości do wszystkich klientów.

Synchronizacja wątków – użycie mutexa

W celu zapewnienia bezpieczeństwa przy dostępie do wspólnej listy klientów (std::vector<Client>), zastosowano std::mutex oraz std::lock_guard.

Mutex chroni operacje dodawania, usuwania oraz iteracji po liście klientów, zapobiegając konfliktom i błędom wynikającym z jednoczesnego dostępu wielu wątków.

std::lock_guard gwarantuje automatyczne odblokowanie mutexa po zakończeniu danego bloku kodu.

Usuwanie klientów – po rozłączeniu klienta jest on usuwany z listy, a pozostali użytkownicy otrzymują stosowną informację.

### Klient
Inicjalizacja WinSock – przygotowanie do połączenia.

Utworzenie gniazda TCP – połączenie z serwerem (127.0.0.1:8080).

Wysłanie nazwy użytkownika – identyfikacja w czacie.

Odbieranie wiadomości – w osobnym wątku (std::thread).

Wysyłanie wiadomości – w głównym wątku.

Zamykanie połączenia – po wpisaniu komendy /exit.

# 🔄 Działanie
### Serwer
Po uruchomieniu tworzy gniazdo i nasłuchuje na porcie 8080.

Dla każdego nowego klienta tworzy osobny wątek.

Odbiera wiadomości i rozsyła je do wszystkich uczestników czatu.

Po rozłączeniu klienta usuwa go z listy i powiadamia innych.

### Klient
Łączy się z serwerem i podaje swój nick.

W osobnym wątku odbiera wiadomości od innych użytkowników.

W głównym wątku umożliwia wpisywanie i wysyłanie wiadomości.

Po wpisaniu /exit zamyka połączenie.
# SO_2_2
