#ifndef DATACACHE_H
#define DATACACHE_H

#include "cache.h"
#include "directory.h"

#include <string>

#include <QDebug>

class Processor;

class DataCache : public Cache
{
    enum MemoryOperation
    {
        load,
        store,
    };

private:
    /// Estructura que representa los valores de la caché de datos.
    int cacheMem[4][4] = {{0}};
public:
    friend Processor;

    DataCache();

    /**
     * @brief getDataAt obtiene el dato de la dirección de memoria especificada. Si es necesario, en este método se resuelven fallos de caché.
     * @param processor puntero al procesador que es mi dueño. Se utiliza para pasárselo a solveFail, en caso de ocuparlo.
     * @param dataPosition posición de memoria que el procesador está pidiendo.
     * @return el valor asociado a la posición de memoria dataPosition.
     */
    int getDataAt(Processor* processor, int dataPosition);

    /**
     * @brief storeDataAt guarda el dato que el procesador especifica en memoria (en la caché, dejando el bloque en modificado). Resuelve fallos de caché en caso que sea necesario.
     * @param processor puntero al procesador que es mi dueño. Se utiliza para pasárselo a solveFail, en caso de ocuparlo. También se utiliza para verificar el rl en caso de que isSc sea verdadero.
     * @param dataPosition posición de memoria en el que el procesador quiere guardar.
     * @param word valor que se quiere guardar en memoria.
     * @param isSc booleano que especifica si la instrucción es un store conditional.
     * @return true si se guardó el valor deseado. False si no se logró guardar el valor deseado. No se guarda el valor deseado en caso de que isSc sea verdadero y el rl de processor no concuerda con dataPosition.
     */
    bool storeDataAt(Processor* processor, int dataPosition, int word, bool isSc = false);

    /**
     * @brief solveFail en caso de que el bloque víctima esté modificado, copia el bloque a memoria y modifica el directorio. Si está compartido, solo modifica el directorio. Luego de eso, llama a un método que obtiene el bloque deseado y luego actualiza el blockID y el state del bloque de la caché.
     * @param processor puntero al procesador que es mi dueño. Se utiliza para avanzar ciclos de reloj y procesar mensajes mientras se obtiene el directorio del bloque víctima.
     * @param blockInMemory número de bloque en memoria que se quiere subir a caché.
     * @param victimBlock posición del bloque víctima en caché.
     * @param operation especifica si es una lectura o una escritura, para actualizar el estado del bloque en caché y en el directorio acorde.
     */
    void solveFail(Processor* processor, const int &blockInMemory, const int &victimBlock, MemoryOperation operation);

    /**
     * @brief copyBlockToMem copia el bloque de caché a la memoria de datos.
     * @param processor puntero al procesador que es mi dueño. Se utiliza para acceder a las memorias de datos de los tres procesadores.
     * @param blockInMemory número de bloque en memoria al que se quiere bajar la información de la caché.
     * @param modifiedBlock posición del bloque en caché que se quiere bajar a memoria.
     * @param copyToAnotherCache booleano que especifica si se debe copiar en la caché de otro procesador.
     * @param otherCacheBlock puntero al bloque en la caché del otro procesador al que se quiere pasar el bloque.
     */
    void copyBlockToMem(Processor* processor, const int &blockInMemory, const int &modifiedBlock, bool copyToAnotherCache = false, int* otherCacheBlock = nullptr);

    /**
     * @brief copyBlockFromMem copia un bloque de memoria a la caché.
     * @param processor puntero al procesador que es mi dueño. Se utiliza para acceder a las memorias de datos de los tres procesadores.
     * @param blockInMemory número de bloque en memoria del que se quiere copiar a caché.
     * @param blockInCache posición del bloque en caché al que se copia de memoria.
     */
    void copyBlockFromMem(Processor *processor, const int &blockInMemory, const int &blockInCache);

    /**
     * @brief changeDirectoryState cambia el directorio de acuerdo al cambio del estado en caché. Si se está invalidando el bloque, marca con un cero en el directorio y verifica si todos son ceros, para marcar el directorio como uncached. En caso de ser compartido o modificado, nada más pone ese estado en el directorio y pone un uno en la posición del procesador.
     * @param processor puntero al procesador que es mi dueño. Se utiliza para acceder a los directorios de cada procesador.
     * @param blockInMemory número de bloque en memoria del que se quiere cambiar el estado.
     * @param blockNewState nuevo estado del bloque en caché, para hacer los cambios en el directorio de manera acorde.
     */
    void changeDirectoryState(Processor* processor, const int &blockInMemory, states blockNewState);

    /**
     * @brief obtainBlock obtiene el bloque deseado, ya sea obteniéndolo de memoria o de la caché de otro procesador.
     * @param processor puntero al procesador que es mi dueño. Se utiliza para acceder a los directorios de cada procesador y acceder a la cola de mensajes de los otros procesadores.
     * @param blockInMemory número de bloque en memoria que se quiere obtener.
     * @param blockInCache posición del bloque en caché al que se copia el bloque actualizado.
     * @param operation especifica si es una lectura o escritura para actualizar caché, directorio y mandar mensajes a los otros procesadores de manera acorde.
     */
    void obtainBlock(Processor* processor, const int &blockInMemory, const int &blockInCache, MemoryOperation operation);

    /**
     * @brief isWordInCache verifica si un bloque se encuentra en la caché.
     * @param blockInMemory número de bloque en memoria que se está verificando.
     * @param blockInCache posición del bloque en caché.
     * @return true si el bloque se encuentra en caché. False si el bloque no se encuentra en caché.
     */
    inline bool isWordInCache(const int &blockInMemory, const int &blockInCache)
    {
        return blockID[blockInCache] == blockInMemory;
    }

    QString toString()
    {
        QString all;

        QString block;
        for(size_t index = 0; index < 4; ++index)
        {
            switch(state[index])
            {
                case invalid:
                    block = "invalid";
                    break;
                case shared:
                    block = "shared";
                    break;
                case modified:
                    block = "modified";
                    break;
            }
            block += " Block[" + QString::number(blockID[index]) + "]";
            for(size_t blockIndex = 0; blockIndex < 4;  ++blockIndex)
                block += " " + QString::number(cacheMem[index][blockIndex]);

            all += " ";
            all.append(block);
            all += " |";
            //qDebug() << all;
        }
        return all;
    }
};

#endif // DATACACHE_H
