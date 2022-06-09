# TP4 SISCOM

## Desarrollo

Lo que hace el presente modulo es poner el pin 24 como salida y colocarlo en un estado inicial alto. En el pin 24 hay conectado un LED a traves de GND. 
Se habilitaron las interrupciones para el pin 23, lo que hace que al cargarse el modulo la rasberry no tenga que hacer polling, si no que cuando hay un **rising edge** en el pin 23 este genera una interrupcion y hace un toggle al pin 24. En el pin 23 hay un push button conectado a traves de Vcc.

Falta hacer un retraso antirrebote para el push button, ya que cada vez que se aprieta el boton se llama varias veces a la rutina de interrupcion.
