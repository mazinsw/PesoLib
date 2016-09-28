package br.com.mzsw;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * Biblioteca para conexão com balanças.
 * obtém o peso de balanças, realiza conexão automática
 * 
 * @author Mazin
 *
 */
public class PesoLib implements Runnable {
	private List<BalancaListener> listeners;
	private PesoLibWrapper driver;
	private long instance;
	private boolean canceled;
	private int ultimoPeso;
	
	/**
	 * Cria uma conexão com uma balança
	 * 
	 * @param configuracao
	 */
	public PesoLib() {
		listeners = new ArrayList<>();
		driver = new PesoLibWrapper();
		instance = driver.criar("");
		if(instance == 0)
			throw new RuntimeException("Não foi possível criar uma instância da biblioteca");
		Thread thread = new Thread(this);
		thread.start();
	}
	
	/**
	 * Cria uma conexão com uma balança especificando configuração de porta e tempo
	 * 
	 * @param configuracao
	 */
	public PesoLib(String configuracao) {
		listeners = new ArrayList<>();
		driver = new PesoLibWrapper();
		instance = driver.criar(configuracao);
		if(instance == 0)
			throw new RuntimeException("Não foi possível criar uma instância da biblioteca");
		Thread thread = new Thread(this);
		thread.start();
	}
	
	/**
	 * Adiciona uma interface que receberá evento de conexão e recebimento de peso
	 * 
	 * @param l interface que receberá os eventos
	 */
	public void addEventListener(BalancaListener l) {
		listeners.add(l);
	}
	
	/**
	 * Fecha a conexão com a balança, após a chamada desse método, a instancia da classe não poderá mais ser usada
	 * 
	 */
	public void fecha() {
		if(canceled)
			return;
		canceled = true;
		driver.cancela(instance);
		driver.libera(instance);
		instance = 0;
	}
	
	private void needActive() {
		if(canceled)
			throw new RuntimeException("A instancia da biblioteca já foi liberada");	
	}
	
	/**
	 * Informa para a balança o preço do item que está sendo pesado
	 * 
	 * @param preco preço do item
	 */
	public void setPreco(float preco) {
		needActive();
		if(!driver.solicitaPeso(instance, preco))
			throw new RuntimeException("Não foi possível ajustar o preço do item da balaça");
	}
	
	/**
	 * Solicita o peso do item sobre a balança
	 * 
	 */
	public void askPeso() {
		setPreco(0.0f);
	}

	/**
	 * Informa se está conectado à uma balança
	 * 
	 * @return true se está conectado, falso caso contrário
	 */
	public boolean isConectado() {
		if(canceled)
			return true;
		return driver.isConectado(instance);
	}
	
	/**
	 * Ajusta configurações de conexão com a balança e tempo de espera
	 * 
	 * @param configuracao configurações com instruções separadas por ;
	 * exemplo: port:COM3;baund:9600, ajusta a porta e a velocidade de conexão
	 */
	public void setConfiguracao(String configuracao) {
		needActive();
		driver.setConfiguracao(instance, configuracao);
	}
	
	/**
	 * Obtém a configuração da conexão atual
	 * 
	 * @return configurações com instruções separadas por ;
	 * exemplo: port:COM3;baund:9600, o primero é a porta e segundo é a velocidade de conexão 
	 */
	public String getConfiguracao() {
		needActive();
		return driver.getConfiguracao(instance);
	}
	

	/**
	 * Obtém o último peso enviado pela balança
	 * 
	 * @return peso em gramas
	 */
	public int getUltimoPeso() {
		return ultimoPeso;
	}

	/**
	 * Obtém todas as marcas de balanças suportadas pela biblioteca
	 * 
	 * @return lista com as marcas suportadas
	 */
	public List<String> getMarcas() {
		needActive();
		String marcas = driver.getMarcas(instance);
		return Arrays.asList(marcas.split("\r\n"));
	}
	
	/**
	 * Obtém todas os modelos suportados pela balança da marca informada
	 * 
	 * @param marca marca da balança
	 * @return lista com todos os modelos suportados
	 */
	public List<String> getModelos(String marca) {
		needActive();
		String modelos = driver.getModelos(instance, marca);
		return Arrays.asList(modelos.split("\r\n"));
	}
	
	/**
	 * Obtém a versão da biblioteca
	 * 
	 * @return versão no formato 0.0.0.0
	 */
	public String getVersao() {
		return driver.getVersao(instance);
	}
	
	@Override
	public void run() {
		int event;
		
		do {
			event = driver.aguardaEvento(instance);
			switch (event) {
			case PesoLibWrapper.EVENTO_CANCELADO:
				break;
			case PesoLibWrapper.EVENTO_CONECTADO:
				postEventConnect();
				break;
			case PesoLibWrapper.EVENTO_DESCONECTADO:
				postEventDisconnect();
				break;
			default:
				postEventWeightReceived(driver.getUltimoPeso(instance));
			}
		} while(!canceled && event != PesoLibWrapper.EVENTO_CANCELADO);
	}

	private void postEventConnect() {
		for (BalancaListener elem : listeners) {
			elem.onConectado(this);
		}
	}

	private void postEventDisconnect() {
		for (BalancaListener elem : listeners) {
			elem.onDesconectado(this);
		}
	}

	private void postEventWeightReceived(int ultimoPeso) {
		this.ultimoPeso = ultimoPeso;
		for (BalancaListener elem : listeners) {
			elem.onPesoRecebido(this, ultimoPeso);
		}
	}

}
