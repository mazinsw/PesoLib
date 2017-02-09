unit Balanca;

interface

uses
  Windows, SysUtils, Classes, SyncObjs;

const
  pesolib = 'PesoLib.dll';

type
  TPesoEvento = (peCancelado, peConectado, peDesconectado, pePesoRecebido);
  TPesoRecebidoEvent = procedure(Sender: TObject; Gramas: Integer) of object;
  TPesoLib = Pointer;
  TPesoLib_criaFunc = function(const configuracao: PAnsiChar): TPesoLib; stdcall;
  TPesoLib_isConectadoFunc = function(Instancia: TPesoLib): Integer; stdcall;
  TPesoLib_setConfiguracaoFunc = procedure(Instancia: TPesoLib; const configuracao: PAnsiChar); stdcall;
  TPesoLib_getConfiguracaoFunc = function(Instancia: TPesoLib): PAnsiChar; stdcall;
  TPesoLib_getMarcasFunc = function(Instancia: TPesoLib): PAnsiChar; stdcall;
  TPesoLib_getModelosFunc = function(Instancia: TPesoLib; const modelo: PAnsiChar): PAnsiChar; stdcall;
  TPesoLib_aguardaEventoFunc = function(Instancia: TPesoLib): Integer; stdcall;
  TPesoLib_getUltimoPesoFunc = function(Instancia: TPesoLib): Integer; stdcall;
  TPesoLib_recebePesoFunc = function(Instancia: TPesoLib; var Gramas: Integer): Integer; stdcall;
  TPesoLib_solicitaPesoFunc = function(Instancia: TPesoLib; ValorDoQuilo: Single): Integer; stdcall;
  TPesoLib_cancelaFunc = procedure(Instancia: TPesoLib); stdcall;
  TPesoLib_liberaFunc = procedure(Instancia: TPesoLib); stdcall;
  TPesoLib_getVersaoFunc = function(Instancia: TPesoLib): PAnsiChar; stdcall;

  TBalancaWrapper = class
  private
    FPeso: Integer;
    FModulo: THandle;
    FInstancia: TPesoLib;
    FMarcas: TStrings;
    FCancelado: Boolean;
    FThreadEvento: TThread;
    FWaitEvent: TEvent;
    FOnPesoRecebido: TPesoRecebidoEvent;
    FOnConectado: TNotifyEvent;
    FOnDesconectado: TNotifyEvent;
    FPesoLib_cria: TPesoLib_criaFunc;
    FPesoLib_isConectado: TPesoLib_isConectadoFunc;
    FPesoLib_setConfiguracao: TPesoLib_setConfiguracaoFunc;
    FPesoLib_getConfiguracao: TPesoLib_getConfiguracaoFunc;
    FPesoLib_getMarcas: TPesoLib_getMarcasFunc;
    FPesoLib_getModelos: TPesoLib_getModelosFunc;
    FPesoLib_aguardaEvento: TPesoLib_aguardaEventoFunc;
    FPesoLib_getUltimoPeso: TPesoLib_getUltimoPesoFunc;
    FPesoLib_recebePeso: TPesoLib_recebePesoFunc;
    FPesoLib_solicitaPeso: TPesoLib_solicitaPesoFunc; 
    FPesoLib_cancela: TPesoLib_cancelaFunc;
    FPesoLib_libera: TPesoLib_liberaFunc; 
    FPesoLib_getVersao: TPesoLib_getVersaoFunc;
    procedure DoPesoRecebido;
    procedure DoConectado;
    procedure DoDesconectado;
    function GetConectado: Boolean;
    function GetConfiguracao: string; 
    function GetVersao: string;
    function GetUltimoPeso: Integer;
    procedure SetConfiguracao(const Value: string);
  public
    constructor Create(const Biblioteca, Configuracao: string);
    destructor Destroy; override;
    procedure SetPreco(Preco: Currency);
    procedure GetModelos(const Marca: string; Lista: TStrings);
    function AguardaEvento: TPesoEvento;
    procedure Start;
    procedure Cancela;
    property Conectado: Boolean read GetConectado;
    property Configuracao: string read GetConfiguracao write SetConfiguracao;
    property Marcas: TStrings read FMarcas;
    property Versao: string read GetVersao;
    property UltimoPeso: Integer read GetUltimoPeso;

    property OnConectado: TNotifyEvent read FOnConectado write FOnConectado;
    property OnDesconectado: TNotifyEvent read FOnDesconectado write FOnDesconectado;
    property OnPesoRecebido: TPesoRecebidoEvent read FOnPesoRecebido write FOnPesoRecebido;
  end;

  TBalanca = class(TComponent)
  private
    { Private declarations }
    FPeso: Integer;
    FPreco: Currency;
    FAtivo: Boolean;
    FWrapper: TBalancaWrapper;
    FConfiguracao: string;
    FOnPesoRecebido: TPesoRecebidoEvent;
    FOnConectado: TNotifyEvent;
    FOnDesconectado: TNotifyEvent;
    FNomeDriver: string;
    procedure SetPreco(const Value: Currency);
    procedure RequerAtivo;
    procedure SetAtivo(const Value: Boolean);
    procedure SetNomeDriver(const Value: string);
    function GetConfiguracao: string;
    procedure SetConfiguracao(const Value: string);
    function GetVersao: string;
  protected
    { Protected declarations }
    procedure DoPesoRecebido(Sender: TObject; Gramas: Integer);
    procedure DoConectado(Sender: TObject);
    procedure DoDesconectado(Sender: TObject);
  public
    { Public declarations }
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;         
    procedure SolicitaPeso;
    procedure GetMarcas(Lista: TStrings);           
    procedure GetModelos(const Marca: string; Lista: TStrings);
    function Conectado: Boolean;
    property Preco: Currency read FPreco write SetPreco;
    property Versao: string read GetVersao;
    property Configuracao: string read GetConfiguracao write SetConfiguracao;
  published
    property UltimoPeso: Integer read FPeso;
    property Ativo: Boolean read FAtivo write SetAtivo;
    property NomeDriver: string read FNomeDriver write SetNomeDriver;
    property OnConectado: TNotifyEvent read FOnConectado write FOnConectado;
    property OnDesconectado: TNotifyEvent read FOnDesconectado write FOnDesconectado;
    property OnPesoRecebido: TPesoRecebidoEvent read FOnPesoRecebido write FOnPesoRecebido;
  end;

  TThreadEvento = class(TThread)
  private
    FWrapper: TBalancaWrapper;
  protected
    procedure Execute; override;
  public
    constructor Create(Wrapper: TBalancaWrapper);
  end;

procedure Register;

implementation

uses
  Dialogs, AnsiStrings;

procedure Register;
begin
  RegisterComponents('MZSW', [TBalanca]);
end;

{ TBalanca }

function TBalanca.Conectado: Boolean;
begin
  Result := False;
  if not Ativo then
    Exit;
  Result := FWrapper.GetConectado;
end;

constructor TBalanca.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
  FNomeDriver := pesolib;
  FAtivo := False;
  FWrapper := nil;
end;

destructor TBalanca.Destroy;
begin
  SetAtivo(False);
  inherited;
end;

procedure TBalanca.DoConectado(Sender: TObject);
begin
  if Assigned(FOnConectado) then
    FOnConectado(Self);
end;

procedure TBalanca.DoDesconectado(Sender: TObject);
begin
  if Assigned(FOnDesconectado) then
    FOnDesconectado(Self);
end;

procedure TBalanca.DoPesoRecebido(Sender: TObject; Gramas: Integer);
begin
  FPeso := Gramas;
  if Assigned(FOnPesoRecebido) then
    FOnPesoRecebido(Self, Gramas);
end;

function TBalanca.GetConfiguracao: string;
begin
  Result := FConfiguracao;
  if not FAtivo then
    Exit;
  Result := FWrapper.GetConfiguracao;
end;

procedure TBalanca.GetMarcas(Lista: TStrings);
begin
  RequerAtivo;
  Lista.AddStrings(FWrapper.Marcas);
end;

procedure TBalanca.GetModelos(const Marca: string; Lista: TStrings);
begin
  RequerAtivo;
  FWrapper.GetModelos(Marca, Lista);
end;

function TBalanca.GetVersao: string;
begin
  RequerAtivo;
  Result := FWrapper.GetVersao;
end;

procedure TBalanca.RequerAtivo;
begin
  if not FAtivo then
    raise Exception.Create('Driver de balança não ativo');
end;

procedure TBalanca.SetAtivo(const Value: Boolean);
begin
  if Value = FAtivo then
    Exit;
  if not Value then
  begin
    FWrapper.Free;
    FWrapper := nil;
    FAtivo := Value;
    Exit;
  end;
  try
    FWrapper := TBalancaWrapper.Create(FNomeDriver, FConfiguracao);
    FWrapper.OnConectado := DoConectado;
    FWrapper.OnDesconectado := DoDesconectado;
    FWrapper.OnPesoRecebido := DoPesoRecebido;
    FWrapper.Start;
    FAtivo := Value;
  except
    on E: Exception do
    begin
      FWrapper := nil;
      raise;
    end;
  end;
end;

procedure TBalanca.SetConfiguracao(const Value: string);
begin
  if not FAtivo then
  begin
    if Value = FConfiguracao then
      Exit;
    FConfiguracao := Value;
    Exit;
  end;
  FWrapper.SetConfiguracao(Value);
end;

procedure TBalanca.SetNomeDriver(const Value: string);
begin
  if FNomeDriver = Value then
    Exit;
  FNomeDriver := Value;
  if not Ativo then
    Exit;
  Ativo := False;
  Ativo := True;
end;

procedure TBalanca.SetPreco(const Value: Currency);
begin
  RequerAtivo;
  FPreco := Value;
  FWrapper.SetPreco(Value);
end;

procedure TBalanca.SolicitaPeso;
begin
  RequerAtivo;
  FWrapper.SetPreco(0.00);
end;

{ TBalancaWrapper }

function TBalancaWrapper.AguardaEvento: TPesoEvento;
begin
  case FPesoLib_aguardaEvento(FInstancia) of
    0: Result := peCancelado;
    1: Result := peConectado;
    2: Result := peDesconectado;
  else
    Result := pePesoRecebido;
  end;
end;

procedure TBalancaWrapper.Cancela;
begin
  if FCancelado then
    Exit;
  FCancelado := True;
  FPesoLib_cancela(FInstancia);
end;

constructor TBalancaWrapper.Create(const Biblioteca, Configuracao: string);
begin
  FCancelado := True;
  FModulo := LoadLibrary(PChar(Biblioteca));
  if FModulo = 0 then
    raise Exception.CreateFmt('Não foi possível carregar a biblioteca %s', [Biblioteca]);
  FPesoLib_cria := TPesoLib_criaFunc(GetProcAddress(FModulo, 'PesoLib_cria'));
  FPesoLib_isConectado := TPesoLib_isConectadoFunc(GetProcAddress(FModulo, 'PesoLib_isConectado'));
  FPesoLib_setConfiguracao := TPesoLib_setConfiguracaoFunc(GetProcAddress(FModulo, 'PesoLib_setConfiguracao'));
  FPesoLib_getConfiguracao := TPesoLib_getConfiguracaoFunc(GetProcAddress(FModulo, 'PesoLib_getConfiguracao'));
  FPesoLib_getMarcas := TPesoLib_getMarcasFunc(GetProcAddress(FModulo, 'PesoLib_getMarcas'));
  FPesoLib_getModelos := TPesoLib_getModelosFunc(GetProcAddress(FModulo, 'PesoLib_getModelos'));
  FPesoLib_aguardaEvento := TPesoLib_aguardaEventoFunc(GetProcAddress(FModulo, 'PesoLib_aguardaEvento'));
  FPesoLib_getUltimoPeso := TPesoLib_getUltimoPesoFunc(GetProcAddress(FModulo, 'PesoLib_getUltimoPeso'));
  FPesoLib_recebePeso := TPesoLib_recebePesoFunc(GetProcAddress(FModulo, 'PesoLib_recebePeso'));
  FPesoLib_solicitaPeso := TPesoLib_solicitaPesoFunc(GetProcAddress(FModulo, 'PesoLib_solicitaPeso'));
  FPesoLib_cancela := TPesoLib_cancelaFunc(GetProcAddress(FModulo, 'PesoLib_cancela'));
  FPesoLib_libera := TPesoLib_liberaFunc(GetProcAddress(FModulo, 'PesoLib_libera'));
  FPesoLib_getVersao := TPesoLib_getVersaoFunc(GetProcAddress(FModulo, 'PesoLib_getVersao'));
  FInstancia := FPesoLib_cria(PAnsiChar(AnsiString(Configuracao)));
  FCancelado := False;
  FMarcas := TStringList.Create;
  FMarcas.Text := string(FPesoLib_getMarcas(FInstancia));
  FWaitEvent := TEvent.Create(nil, False, True, '');
  FThreadEvento := TThreadEvento.Create(Self);
end;

destructor TBalancaWrapper.Destroy;
begin
  Cancela;
  FWaitEvent.WaitFor(INFINITE);
  FPesoLib_libera(FInstancia);
  FWaitEvent.Free;
  FMarcas.Free;
  FreeLibrary(FModulo);
  inherited;
end;

function TBalancaWrapper.GetConectado: Boolean;
begin
  Result := FPesoLib_isConectado(FInstancia) <> 0;
end;

function TBalancaWrapper.GetConfiguracao: string;
begin
  Result := string(AnsiStrings.StrPas(FPesoLib_getConfiguracao(FInstancia)));
end;

procedure TBalancaWrapper.GetModelos(const Marca: string; Lista: TStrings);
var
  L: TStrings;
begin
  L := TStringList.Create;
  L.Text := string(FPesoLib_getModelos(FInstancia, PAnsiChar(AnsiString(Marca))));
  Lista.AddStrings(L);
  L.Free;
end;

function TBalancaWrapper.GetUltimoPeso: Integer;
begin
  Result := FPesoLib_getUltimoPeso(FInstancia);
end;

function TBalancaWrapper.GetVersao: string;
begin
  Result := string(FPesoLib_getVersao(FInstancia));
end;

procedure TBalancaWrapper.SetConfiguracao(const Value: string);
begin
  FPesoLib_setConfiguracao(FInstancia, PAnsiChar(AnsiString(Value)));
end;

procedure TBalancaWrapper.SetPreco(Preco: Currency);
begin
  FPesoLib_solicitaPeso(FInstancia, Preco);
end;

procedure TBalancaWrapper.Start;
begin
  FThreadEvento.Start;
end;

procedure TBalancaWrapper.DoConectado;
begin
  if Assigned(FOnConectado) then
    FOnConectado(Self);
end;

procedure TBalancaWrapper.DoDesconectado;
begin
  if Assigned(FOnDesconectado) then
    FOnDesconectado(Self);
end;

procedure TBalancaWrapper.DoPesoRecebido;
begin
  if Assigned(FOnPesoRecebido) then
    FOnPesoRecebido(Self, FPeso);
end;

{ TThreadEvento }

constructor TThreadEvento.Create(Wrapper: TBalancaWrapper);
begin
  inherited Create(True);
  FreeOnTerminate := True;
  FWrapper := Wrapper;
end;

procedure TThreadEvento.Execute;
var
  Evento: TPesoEvento;
begin
  FWrapper.FWaitEvent.ResetEvent;
  while not FWrapper.FCancelado do
  begin
    Evento := FWrapper.AguardaEvento;
    case Evento of
      peCancelado: ;
      peConectado: Synchronize(FWrapper.DoConectado);
      peDesconectado: Synchronize(FWrapper.DoDesconectado);
    else
      FWrapper.FPeso := FWrapper.UltimoPeso;
      Synchronize(FWrapper.DoPesoRecebido);
    end;
  end;
  FWrapper.FWaitEvent.SetEvent;
end;

end.
 