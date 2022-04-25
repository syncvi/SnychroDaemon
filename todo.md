[12p.]lub plik w katalogu źrodłowym ma
późniejszą datę ostatniej modyfikacji demon wykonuje kopię pliku z katalogu
źródłowego do katalogu docelowego - ustawiając w katalogu docelowym datę
modyfikacji tak aby przy kolejnym obudzeniu nie trzeba było wykonać kopii
(chyba ze plik w katalogu źródłowym zostanie ponownie zmieniony). Jeżeli zaś
odnajdzie plik w katalogu docelowym, którego nie ma w katalogu źródłowym to
usuwa ten plik z katalogu docelowego. Możliwe jest również natychmiastowe
obudzenie się demona poprzez wysłanie mu sygnału SIGUSR1. 
Dodatkowo:
a) [10p.] Dodatkowa opcja -R pozwalająca na rekurencyjną synchronizację
katalogów (teraz pozycje będące katalogami nie są ignorowane). W szczególności
jeżeli demon stwierdzi w katalogu docelowym podkatalog którego brak w
katalogu źródłowym powinien usunąć go wraz z zawartością.
b) [12p.] W zależności od rozmiaru plików dla małych plików wykonywane jest
kopiowanie przy pomocy read/write a w przypadku dużych przy pomocy
mmap/write (plik źródłowy) zostaje zamapowany w całości w pamięci. Próg
dzielący pliki małe od dużych może być przekazywany jako opcjonalny argument. 
