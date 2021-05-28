# Progetto Sistemi Operativi 2020-2021

### Dennis Rotondi, buddy allocator con la bitmap.

## what 

Il progetto consiste nel realizzare un allocatore di memoria che riesce a gestire un buffer fornitogli utilizzando esclusivamente una bitmap. 
A tal fine mi sono basato su quanto spiegatoci durante la lezione degli allocatori. Ho pensato alla bitmap come ad un albero binario completo dove il nodo radice (per facilità di visualizzazione ho posto tale nodo all'indice 0 modificando alcune delle funzioni già fornite) rappresenta il buffer completamente allocato in un unico buddy, quelli al livello 1, buddy 1 e 2, rappresentano dei buddy di dimensione pari alla metà del buffer e così via dimezzando ad ogni livello la dimensione di quello precedente.  

## how 

Ogni volta che voglio allocare un blocco di dimensione size cerco nel livello che contiene i blocchi più piccoli per contenerlo, in caso ce ne sia uno disponibile ritorno il puntatore ad una porzione del buffer assicurandomi di salvare nei -4byte di questo l'indice del bit nella bitmap che rappresenta il buddy, altrimenti segnalo l'impossibilità attraverso una print e ritorno NULL.  
Per mantenere consistente la bitmap ho fatto in modo che ogni volta che prendo un blocco al livello desiderato setto ad 1, quindi occupati, i bit dei buddy padre (tutti quelli che avrebbero potuto originale il buddy che ho preso partizionandosi) e quelli dei figli (ottenibili partizionandolo), ora il buddy serve in quello stato e non potrà piu' essere partizionato per altri livelli. 
Quando si fa la free si risale all'indice precedentemente nascosto poco prima dell'indirizzo ritornato e, nel caso sia un indirizzo valido e non si stia facendo una double free, si settera' disponibile lui e tutti i suoi figli, nel caso anche il suo buddy sia libero verrà unito e renderà libero anche suo padre, questo ricorsivamente finché si arriverà alla radice o il padre appena liberato abbia il buddy occupato. 

## how-to-run

Il file buddy_allocator_test.c contiene il main e può essere modificato a piacimento pensando di avere, una volta inizializzato, un allocatore che gestisce l'heap. Per compilare usare il makefile e lanciare l'elf generato dalla compilazione per vedere la gestione della memoria.

![alt text](https://i.imgur.com/oWeQUiD.png)
