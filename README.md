# trabalho-de-redes-1

====== Mensageiro Local ======

Neste trabalho, o objetivo é a criação de um mensageiro: Um aplicativo que possibilita o envio de mensagens, similar ao WhatsApp, Telegram, Signal, Skype, ICQ e MSN Messenger.
Este mensageiro será composto de um servidor e de um cliente que rodam em computadores diferentes e se comunicam através de um cabo Ethernet. O mensageiro deverá ser capaz de enviar mensagens de texto e também de trocar arquivos de mídia. O servidor precisa funcionar com apenas um cliente, e um cliente se conecta a apenas um servidor, sendo fora do escopo do trabalho a implementação de um mensageiro que suporta mais de duas pessoas.

===== Implementação do trabalho =====
==== Mensagens de texto ====
Os dois usuários devem ser capazes de fazer envios de mensagem de texto com todos os caracteres disponíveis na [[http://wiki.inf.ufpr.br/maziero/doku.php?id=prog2:codificacao_de_caracteres#a_codificacao_utf-8|codificação UTF-8]]. Isso inclui por exemplo letras acentuadas e emojis.

As mensagens devem ser enviadas utilizando para-espera, onde próxima mensagem só é enviada após a confirmação (ack) da mensagem anterior.

==== Arquivos de mídia ====
Os dois usuários devem ser capazes de fazer envios de qualquer tipo de arquivo de mídia, independente do formato e do tamanho do arquivo.

Visto que arquivos de mídia podem ter tamanhos arbitrários, o uso de janelas deslizantes é necessário independente da quantidade de arquivos enviados de uma só vez.

===== Definições de implementação =====

  * Mensagens de texto enviadas devem ser apresentada no formato:
<code>
 [<data e hora>]<Usuário> : mensagem
</code>

  * Cada mensagem enviada deve ser apresentada separadamente
  * Em mídias enviadas, não é necessário apresentar a mídia, apenas notificar o usuário que uma mídia foi enviada e o local em que pode ser encontrada. Sugestão:
<code>
[<data e hora>]<Usuário> : "Arquivo de midia enviado! O arquivo pode ser encontrado em <caminho_para_arquivo>"
</code>
  * De forma semelhante ao editor de texto modal vi ou de atalhos emacs, o envio de mensagens e mídias pode ser feito via comandos. Por exemplo:
      * Como vi 
          * ''i'': Inicia a criação de uma mensagem. Enter para enviar.
          * ''<esc>'': Sai do modo de inserção de uma mensagem.
          * '':q<enter>'': Sai do programa.
          * '':send x<enter>'': Envia arquivo x.
      * Como emacs
          * Sempre em modo de criação de uma mensagem. Enter para enviar.
          * ''<control-e>'': Inicia inserção de um arquivo para envio. Enter para enviar.
          * ''<control-q>'': Sai do programa.


===== Formato da mensagem de comunicação =====

O protocolo é inspirado no [[https://en.wikipedia.org/wiki/Kermit_%28protocol%29|Kermit]] e possui os seguintes campos, na ordem que eles são enviados na rede:

^Marcador de início	^Tipo	^Sequência	^Tamanho	^Data	^Paridade^
| 8 bits | 6 bits | 4 bits | 6 bits | n bytes | 8 bits |

A memória é endereçada por byte, então alguns campos serão contemplados por partes de alguns bytes, podendo também utilizar vários bytes. Sugere-se a implementação utilizando [[https://wiki.inf.ufpr.br/maziero/doku.php?id=prog2:operacoes_com_bits|bit fields do C ou C++ ou operações com bits]]. Os tipos de cada um dos campos serão definidos em aula e seguem.

  * Marcador de início: A sequência de bits ''01111110'' (126 em decimal, ou 0x7e em hexadecimal).
  * Tipo: Pode ser um de
      * **texto**:
      * **mídia**:
      * **ack**:
      * **nack**:
      * **erro**:
      * **inicio de transmissão**:
      * **fim de transmissão**:
      * **dados**:
  * Sequência: A sequência local da mensagem, devendo ser incrementada pela própria máquina ao enviar. Esta sequência varia de 0 a 15 de forma circular.
  * Tamanho: Tamanho do campo de dados, não incluindo nenhum outro campo do protocolo, variando de 0 a 63 bytes. Isso significa que o tamanho máximo de uma mensagem é de 63+3 = 66 bytes.
  * Dados: Campo definido de acordo com o tipo da mensagem e de tamanho definido pelo campo anterior.
      * **texto**: este campo codifica o texto da mensagem em UTF-8.
      * **mídia**: este campo codifica o início do envio de um arquivo, e deve conter o tamanho do arquivo em bytes utilizando 4 bytes.
      * **ack**: este campo codifica até qual sequência foi aceita (inclusive) em 1 byte.
      * **nack**: este campo codifica qual sequência não foi recebida corretamente em 1 byte, e pede retransmissão da mesma.
      * **erro**: este campo codifica um erro em 1 byte da enumeração:
          * **diretório inexistente (local de salvamento das mídias)**:
          * **sem permissão para salvar no diretório**:
          * **espaço insuficiente para arquivo**:

Ao se codificar inteiros em vários bytes, utilize a codificação //[[https://en.wikipedia.org/wiki/Endianness|little-endian]]//, isto é, o byte menos significativo vem **primeiro**. Note que isto é diferente do padrão da rede. O padrão da rede é utilizar a codificação //big-endian// (e a função ''htons'' é utilizada para normalizar para //big-endian//), onde os bytes mais significativos vem primeiro (por exemplo, um IP ''127.0.0.1'' seria codificado como ''0x7f 0x00 0x00 0x01''). Se optou por esta escolha simplesmente por uma questão pragmática: Os processadores com que trabalhamos são em sua maioria //little-endian//.

===== Requisitos não funcionais =====
  * O trabalho deve ser escrito em C ou C++.
  * Devem ser utilizados [[raw_socket|sockets crus]].
  * É necessário o uso de //timeout//.
  * É necessário permissão root para execução do trabalho. Os computadores do DINF não aceitam comandos root por parte dos alunos, logo é necessário ou levar pendrives bootáveis, ou computador próprio para a apresentação do trabalho.

===== Diagramas de Sequência =====
----
Criar diagramas de sequencia

===== Sugestões de implementação ======

    * Para verificar se uma sequência vem depois da outra, pense na diferença das sequências: É grande o suficiente pra estarem no meio, ou nas bordas? O algoritmo que você vai acabar criando é aquele de [[https://en.wikipedia.org/wiki/Serial_number_arithmetic|aritmética de números seriais]].
    * Para verificar quanto espaço livre uma máquina tem (lembre-se também de ter uma certa tolerância), use a função ''statvfs'' (utilizando como caminho, onde se pretende salvar os arquivos que forem recebidos). O espaço livre em bytes é dado por ''st.f_bsize * st.f_bavail'', onde ''st'' é a estrutura ''statvfs''.
    * Para descobrir que o arquivo é regular e o tamanho do arquivo, utilize a função ''stat''. O tamanho do arquivo em bytes pode ser encontrado em ''st.st_size'' onde ''st'' é a estrutura ''stat''.
    * Note onde há troca de responsabilidade de enviar ACKs e NACKs. Sempre apenas uma das partes (servidor ou cliente) é responsável pelo seu envio, mas pode acontecer delas não saberem que precisam trocar de responsabilidade. Recomenda-se armazenar a última mensagem recebida e a última mensagem enviada caso seja necessário repeti-la.
