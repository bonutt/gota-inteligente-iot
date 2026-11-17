# Registro de uso de IA (prompts)

Conforme os combinados da Global Solution, a IA foi usada como **copiloto**: para gerar uma primeira versão do código, revisar e explicar. A equipe revisou o resultado e sabe explicar cada parte.

**Ferramenta:** Claude (Anthropic)

## Prompts utilizados

1. Envio do enunciado de Physical Computing (circuito ESP32 no Wokwi integrado à CitizenTech) e da ideia da equipe (Gota Inteligente: irrigação inteligente para o agricultor familiar), pedindo um circuito que agregasse valor real à solução, e não um projeto paralelo.
2. Pedido para que o firmware seguisse as mesmas regras de decisão do restante da solução (cruzar umidade do solo com a previsão de chuva) e funcionasse sem internet.
3. Pedido de testes dos cenários (solo seco com e sem chuva, solo crítico, solo úmido, irrigação manual e previsão online) e de roteiro para o vídeo.

## O que foi revisado pela equipe

- [ ] Ligação de cada componente aos pinos do ESP32
- [ ] Regras de decisão e limites (35%, 70%, 15%, 70% de chuva)
- [ ] Execução da simulação no wokwi.com e gravação do vídeo
