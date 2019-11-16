/*
    PesoLib - Biblioteca para obtenção do peso de itens de uma balança
    Copyright (C) 2010-2014 MZSW Creative Software

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    MZSW Creative Software
    contato@mzsw.com.br
*/

/** @file PesoLib.h
 *  Main include header for the PesoLib library
 */

#ifndef _PESOLIB_H_
#define _PESOLIB_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum PesoLibEvento
{
	/** A instância da conexão com a balança está sendo liberada */
	Evento_Cancelado = 0,
	/** Conexão estabelecida com uma balança */
	Evento_Conectado,
	/** A balança foi desconectada */
	Evento_Desconectado,
	/** A balança enviou o peso para o computador */
	Evento_PesoRecebido,
	/** A balança está com peso instável */
	Evento_PesoInstavel
} PesoLibEvento;

typedef struct PesoLib PesoLib;

#ifdef BUILD_DLL
# define LIBEXPORT __declspec(dllexport)
#else
#  ifdef LIB_STATIC
#    define LIBEXPORT
#  else
#    define LIBEXPORT extern
#  endif
#endif
#define LIBCALL __stdcall

#include "private/Plataforma.h"

/**
 * Inicia a conexão com uma nova balança
 * 
 * parâmetros
 *   config: configuração da porta
 * 
 * retorno
 *   um ponteiro para a conexão com uma balança
 */
LIBEXPORT PesoLib * LIBCALL PesoLib_cria(const char* config);

/**
 * Verifica se foi estabelecido uma conexão com uma balança
 * 
 * parâmetros
 *   lib: ponteiro para a conexão com uma balança
 * 
 * retorno
 *   um ponteiro para a conexão com uma balança
 */
LIBEXPORT int LIBCALL PesoLib_isConectado(PesoLib * lib);

/**
 * Altera a configuração de conexão com a balança
 * 
 * parâmetros
 *   lib: ponteiro para a conexão com uma balança
 *   config: nova configuração, contendo informações da conexão
 */
LIBEXPORT void LIBCALL PesoLib_setConfiguracao(PesoLib * lib, const char * config);

/**
 * Obtém a configuração atual da conexão com a balabça
 * 
 * parâmetros
 *   lib: ponteiro para a conexão com uma balança
 * 
 * retorno
 *   uma lista de parâmetros de conexão separados por ;
 */
LIBEXPORT const char* LIBCALL PesoLib_getConfiguracao(PesoLib * lib);

/**
 * Obtém todas as marcas suportadas pela biblioteca
 * 
 * parâmetros
 *   lib: ponteiro para a conexão com uma balança
 * 
 * retorno
 *   uma lista de marcas de balanças separadas por quebra de linha \r\n
 */
LIBEXPORT const char* LIBCALL PesoLib_getMarcas(PesoLib * lib);

/**
 * Obtém todos os modelos de balanças suportadas de uma determinada marca
 * 
 * parâmetros
 *   lib: ponteiro para a conexão com uma balança
 *   marca: nome da marca a qual deseja-se obter os modelos
 * 
 * retorno
 *   uma lista de modelos de balanças separadas por quebra de linha \r\n
 */
LIBEXPORT const char* LIBCALL PesoLib_getModelos(PesoLib * lib, const char* marca);

/**
 * Aguarda um evento de conexão ou de recebimento de dados
 * 
 * parâmetros
 *   lib: ponteiro para a conexão com uma balança
 * 
 * retorno
 *   0 se a instancia da conexão foi liberada,
 *   1 se uma conexão foi estabelecida com uma balança,
 *   2 se a balança foi desconectada
 *   3 quando a balança envia o peso para o computador
 */
LIBEXPORT int LIBCALL PesoLib_aguardaEvento(PesoLib * lib);

/**
 * Obtém o último peso recebido da balança
 * 
 * parâmetros
 *   lib: ponteiro para a conexão com uma balança
 * 
 * retorno
 *   último peso recebido pela balança
 */
LIBEXPORT int LIBCALL PesoLib_getUltimoPeso(PesoLib * lib);

/**
 * Aguarda o envio do peso pela balança
 * 
 * parâmetros
 *   lib: ponteiro para a conexão com uma balança
 *   gramas: variável que recebe o peso em gramas, fornecido pela balança
 * 
 * retorno
 *   1 se recebeu o peso ou 0 se a conexão foi finalizada
 */
LIBEXPORT int LIBCALL PesoLib_recebePeso(PesoLib * lib, int* gramas);

/**
 * Solicita o envio do peso pela balança e informa o preço do kilo 
 * para ser mostrado no visor da mesma
 * 
 * parâmetros
 *   lib: ponteiro para a conexão com uma balança
 *   preco: preço do item por quilo que está sendo pesado
 * 
 * retorno
 *   1 se enviou a solicitação com sucesso ou 0 se não existe balança conectada
 */
LIBEXPORT int LIBCALL PesoLib_solicitaPeso(PesoLib * lib, float preco);

/**
 * Cancela uma conexão e libera as funções que aguardam evento
 * 
 * parâmetros
 *   lib: ponteiro para a conexão com uma balança
 */
LIBEXPORT void LIBCALL PesoLib_cancela(PesoLib * lib);

/**
 * Finaliza uma conexão com uma balança
 * 
 * parâmetros
 *   lib: ponteiro para a conexão com uma balança
 */
LIBEXPORT void LIBCALL PesoLib_libera(PesoLib * lib);


/**
 * Obtém a versão da biblioteca
 * 
 * parâmetros
 *   lib: ponteiro para a conexão com uma balança
 */
LIBEXPORT const char* LIBCALL PesoLib_getVersao(PesoLib * lib);

#ifdef __cplusplus
}
#endif

#endif /* _PESOLIB_H_ */