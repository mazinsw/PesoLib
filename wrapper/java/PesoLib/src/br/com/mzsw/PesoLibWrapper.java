package br.com.mzsw;

public class PesoLibWrapper {
	/** Evento acionado quando o metodo cancela for chamado */
	public static final int EVENTO_CANCELADO = 0;
	/** Evento acionado quando uma conexão com uma balança for estabelecida */
	public static final int EVENTO_CONECTADO = 1;
	/** Evento acionado quando uma balança for desconectada */
	public static final int EVENTO_DESCONECTADO = 2;
	/**
	 * Evento acionado quando o peso for recebido por uma solicitação ou caso a
	 * balança tenha enviado
	 */
	public static final int EVENTO_PESORECEBIDO = 3;

	public native long criar(String configuracao);

	public native boolean isConectado(long instancia);

	public native void setConfiguracao(long instancia, String configuracao);

	public native String getConfiguracao(long instancia);

	public native String getMarcas(long instancia);

	public native String getModelos(long instancia, String marca);

	public native int aguardaEvento(long instancia);

	public native int getUltimoPeso(long instancia);

	public native boolean solicitaPeso(long instancia, float preco);

	public native void cancela(long instancia);

	public native void libera(long instancia);

	public native String getVersao(long instancia);

}
