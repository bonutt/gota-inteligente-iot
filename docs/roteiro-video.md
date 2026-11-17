# Roteiro do vídeo — Gota Inteligente (Physical Computing)

O enunciado pede que o vídeo: **(1)** apresente brevemente a solução do grupo, **(2)** explique como o circuito com ESP32 foi incorporado ao projeto e **(3)** demonstre o funcionamento no Wokwi. Duração sugerida: **3 a 4 minutos**.

## Antes de gravar

- [ ] Abra o projeto: https://wokwi.com/projects/480869188509119489 (simulação parada).
- [ ] Deixe os dois controles deslizantes **na direita (0%)**. Neles, **direita = 0%** e **esquerda = 100%**.
- [ ] Deixe a chave de modo na posição **simulada** (como está ao abrir o projeto).
- [ ] Aumente o zoom do navegador para o circuito aparecer grande.
- [ ] Para gravar: **Win + Alt + R** (barra de jogos do Windows) inicia e para a gravação. Ou use o OBS.

---

## 1. A solução do grupo (0:00 – 0:40)

**Tela:** README do GitHub, na Ficha da CitizenTech.

> "Olá! Somos o grupo da **Gota Inteligente**, nossa CitizenTech da Global Solution.
> O problema que a gente ataca é o do **agricultor familiar**: ele rega no 'olhômetro'. Às vezes liga a bomba e duas horas depois chove, desperdiçando água e energia. Outras vezes vem uma seca repentina e ele perde a plantação.
> A Gota Inteligente é um **assistente de irrigação de baixo custo** que cruza a umidade do solo com a previsão do tempo para decidir quando irrigar. A solução tem um modelo de IA, um app para o agricultor e um painel para a prefeitura. E o **coração** de tudo é o kit IoT com ESP32 que vamos mostrar agora."

## 2. Como o ESP32 entra no projeto (0:40 – 1:40)

**Tela:** circuito no Wokwi. Aponte com o mouse cada parte enquanto fala.

> "Esse é o kit que fica no campo. No centro está o **ESP32**.
> - **Do lado esquerdo**, em cima, estão os sensores: o **sensor de umidade do solo**, que aqui no Wokwi é simulado por esse controle deslizante (o Wokwi não tem sensor de solo; no kit real seria um sensor capacitivo de uns 15 reais), e o controle de **chance de chuva**, usado para demonstrar os cenários.
> - Embaixo deles estão os **LEDs de status**: verde para solo ok, amarelo para 'economia, aguardando a chuva' e vermelho para estresse hídrico crítico.
> - **Do lado direito** fica o **LCD**, que mostra a umidade, a chance de chuva e a decisão para o agricultor. Também tem o **DHT22**, que mede temperatura e umidade do ar, e o **botão de irrigação manual**.
> - **Embaixo** está o **relé**, que liga a bomba d'água. O LED azul representa a bomba funcionando.
> - Essa **chave** escolhe entre a previsão simulada e a **previsão real da internet**, que o ESP32 busca pelo Wi-Fi na API Open-Meteo.
>
> A cada 2 segundos o ESP32 lê os sensores, cruza com a previsão de chuva e decide: **irrigar**, **não irrigar** ou **aguardar a chuva**. Ele também monta a leitura em JSON para mandar ao restante da solução, que alimenta o app e o painel. E se a internet cair, as mesmas regras rodam no próprio ESP32."

## 3. Demonstração (1:40 – 3:30)

**Tela:** clique em ▶ e espere aparecer "conectado!" no monitor serial.

**Cenário 1 — Seca sem chuva (os dois controles na direita, em 0%)**
> "Solo seco e nenhuma chance de chuva: o kit **liga a bomba**. Vejam o LED azul, o relé e o LCD mostrando 'BOMBA LIGADA'. Como o solo está muito seco, o LED vermelho de estresse crítico também acende."

**Cenário 2 — Vai chover, o diferencial (arraste o *solo* para uns 25% e o *chuva* para a esquerda, 90%)**
> "Agora o solo continua seco, mas tem **90% de chance de chuva**. Em vez de regar à toa, o kit **segura a bomba** para economizar água e energia. O LED amarelo de economia acende e o LCD mostra 'AGUARDANDO CHUVA'. Esse é o grande diferencial da Gota Inteligente."

**Cenário 3 — Seca crítica (deixe o *solo* em ~10%, quase na direita, com o *chuva* ainda em 90%)**
> "Mas se o solo chega a um **nível crítico**, ele irriga mesmo com chuva prevista, porque aí o risco é perder a plantação."

**Cenário 4 — Solo úmido (arraste o *solo* para a esquerda, ~80%)**
> "Com o solo úmido, ele simplesmente **não irriga**, e o LED verde mostra que está tudo certo."

**Cenário 5 — Irrigação manual (clique no botão azul MANUAL)**
> "O agricultor sempre tem o controle: com o botão manual ele força **10 segundos de irrigação**."

**Monitor serial (role o monitor embaixo)**
> "Aqui no monitor serial dá para ver o **motivo de cada decisão** e a **telemetria em JSON**, que é o que vai para o backend da solução."

**Cenário 6 — Previsão real (clique na chave de modo)**
> "E com a chave ligada, o ESP32 consulta a **previsão real** da internet para a cidade do sítio. No monitor aparece '(Open-Meteo)' com a chance de chuva de verdade."

## 4. Fechamento (3:30 – 4:00)

**Tela:** volte para o circuito ou para o README.

> "Com o kit, cada irrigação evitada antes da chuva é **água e energia economizadas**, e cada seca é detectada **antes da planta murchar**. É assim que a Gota Inteligente leva tecnologia de baixo custo para o agricultor familiar, contribuindo com os ODS 2, 6 e 13. Obrigado!"

---

## Depois de gravar

1. Suba no YouTube como **Não listado**.
2. Copie o link e troque `LINK_VIDEO` no topo do `README.md` pelo link do vídeo.
3. Dê dois cliques no `publicar_github.bat` de novo para enviar a atualização.
