# Cambios

## v0.2.0-rc8

- Clip duro exclusivamente al final, después de Output: sin recorte previo en Inflate ni saturación suave adicional. Conserva exactamente las muestras bajo el techo.
- IN/OUT ocupa el primer interruptor junto al medidor; K-12/K-14 queda debajo.
- Prueba de regresión en ambos órdenes, Band Split, Mono y dos frecuencias de muestreo: Clip ON equivale exactamente al recorte final de Clip OFF.

## v0.2.0-rc7

- Mayor precisión numérica en el procesamiento multibanda y en el orden B.
- Banda media calculada mediante filtros en cascada para evitar cancelaciones y redondeos innecesarios.
- Tratamiento de silencio revisado para preservar las muestras de entrada distintas de cero.
- Comprobaciones funcionales, automatización, niveles y validación VST3 revisadas.
- Documentación centrada en los controles, la ruta de audio y el uso de D2F.
## v0.2.0-rc6

- Inflator Band Split, de tres bandas, y selector de orden A/B.
- Monitor Mono/Stereo y Delta ON/OFF alineado con la entrada original.
- Dos medidores L/R: picos dBFS arriba y RMS K-12/K-14 abajo.
- Clip con LED violeta y conmutador ON/OFF; techo final también en Mono/Delta.
- Frontal alto y contenido centrado entre los cuatro tornillos. Sub antes de Air; frecuencias alrededor de los aros y alineadas con su indicador. Inflate/Curve y Mid/Side en columnas compactas.
- Nombres, ayudas y README revisados, con guía de cada control y diagramas de ruta.
- Factor de salida, arranque y transiciones medidos con mayor precisión del procesamiento.
- Se conserva la protección de estabilidad de Sub a 192 kHz introducida en rc5.

El selector de escala de medidor anterior cambia de función: ahora es K-12/K-14, con dBFS siempre visible en el medidor superior. Revisar su estado al abrir sesiones anteriores. Se conserva la identidad VST3 y los IDs existentes.

