# Driver for Beckhoff EL3xxx Analog Input Devices

The [`lcec_el3xxx`](../src/devices/lcec_el3xxx.c) driver supports a
wide range of analog input devices and replaces a number of older
single-purpose drivers.

It currently supports 4 basic families of devices:

- EL30xx 12-bit analog input modules
- EL31xx 16-bit analog input modules
- EL32xx 16-bit temperature sensor modules
- EM37xx 12-bit pressure modules

See the [source code](../src/devices/lcec_el3xxx.c) or [device
documentation](devices/) for precise details on which hardware is
currently supported. In general, we aim to support EL3xxx and the
equivalent EJ, EP, and EPP modules, but this is not yet complete.

## How to file a bug

Please file an
[issue](http://github.com/linuxcnc-ethercat/linuxcnc-ethercat/issues)
if you have any problems with this driver, including:

- hardware that should work but doesn't.
- similar hardware that should probably should be supported.
- unexpected limitations.
- features that the hardware supports but the driver does not.

## EL30xx modules and issues

The EL30xx devices are fairly basic.  To configure them, add a line
like this to your `ethercat.xml` file:

```xml
    <slave idx="10" type="EL3068" name="D10"/>
```

This should create a bunch of
`lcec.<MASTERID>.<SLAVENAME>.ain-<CHANNEL>-<TYPE>` pins in LinuxCNC.
The pin types include:

- `bias` -- settable to adjust the value returned in `val`.
- `error` -- true if the hardware has logged an error.
- `overrange` -- true if the input is over range.
- `raw` -- the raw input measurement from the device.
- `scale` -- settable to adjust the value returned in `val`.
- `underrange` -- true if the input is under range.
- `val` -- the current measurement on the input, with `scale` and `bias`
  applied.

Examples:

```
     8  float I/O             0  lcec.0.D10.ain-0-bias
     8  bit   OUT         FALSE  lcec.0.D10.ain-0-error
     8  bit   OUT         FALSE  lcec.0.D10.ain-0-overrange
     8  s32   OUT             0  lcec.0.D10.ain-0-raw
     8  float I/O             1  lcec.0.D10.ain-0-scale
     8  bit   OUT         FALSE  lcec.0.D10.ain-0-underrange
     8  float OUT             0  lcec.0.D10.ain-0-val
     8  float I/O             0  lcec.0.D10.ain-1-bias
     8  bit   OUT         FALSE  lcec.0.D10.ain-1-error
     8  bit   OUT         FALSE  lcec.0.D10.ain-1-overrange
     8  s32   OUT             0  lcec.0.D10.ain-1-raw
     8  float I/O             1  lcec.0.D10.ain-1-scale
     8  bit   OUT         FALSE  lcec.0.D10.ain-1-underrange
     8  float OUT             0  lcec.0.D10.ain-1-val
```

Even though this is a "12-bit device", Beckhoff returns results in the
range of -0x7fff - 0x7fff, which is 16 bits of precision.  For devices
where negative answers don't make sense, this range is actually
0-0x7fff (or 32,767).

The value of `val` is `raw` / 32767 * `scale` + `bias`.  Note that
this means that `val` ranges between 0.0 and 1.0 unless `scale` and
`bias` are changed.

## EL31xx modules and issues

**Note**: It is possible that some older EL31xx devices (for example, EL3104
revision r16 or r17) may not work with this driver.  If you have
hardware that fails to load with a PDO error, please file a bug (see
above for directions).

EL31xx devices should match the description of EL30xx devices above,
with one minor difference: there is also a `sync-err` pin that will
report on distributed clock sync failures.

## EM37xx pressure modules and issues

The Beckhoff EM3701, EM3702, and EM3712 should all be supported.  They
are very similar to EL30xx devices, except the pins are named starting
with `press*` instead of `ain*`.

## EL32xx temperature modules and issues

The driver now supports [Beckhoff EL32xx temperature
modules](https://www.beckhoff.com/en-us/products/i-o/ethercat-terminals/el3xxx-analog-input/?fq=pf_type_signal:562475793).
In general, we should be able to support all of the EL32xx temperature
modules, and probably some of the EL33xx modules, if anyone has
hardware to test them with.  File a bug (see above) if you find
unsupported hardware.

The temperature modules have a few optional settings in `ethercat.xml` to
control temperature-specific featues:

```xml
    <slave idx="16" type="EL3204" name="D16">
      <modParam name="ch0Sensor" value="Pt100"/>
      <modParam name="ch0Resolution" value="High"/>
      <modParam name="ch0Wires" value="2"/>
      <modParam name="ch1Sensor" value="Ohm/16"/>
	  ...
	</slave>
```

There are 3 `<modParam>` settings for each channel:

- `chXSensor`: controls the temperature sensor type attached to the
  channel.  Various sensors have different resistance curves.
  Supported types are:
  - `Pt100`: Pt100-type sensor, -200 to +850C
  - `Pt200`: Pt200-type sensor, -200 to +850C
  - `Pt500`: Pt500-type sensor, -200 to +850C
  - `PT1000`: Pt1000-type sensor, -200 to +850C
  - `Ni100`: Ni100-type sensor, -60 to +250C
  - `Ni120` Ni120-type sensor, -60 to +250C
  - `Ni1000` Ni1000-type sensor, -60 to +250C
  - `Ni1000-TK5000` Ni1000-type sensor that reads 1500 Ohm at 100C, -30 to +160C
  - `Ohm/16` No sensor, read resistance directly.  0-4095 Ohms.
  - `Ohm/64` No sensor, read resistence directly.  0-1023 Ohms.
- `chXResolution`: controls the resolution of returned results.
  - `Signed`: returns results with 0.1C per bit. Default on most
    EL32xxx hardware.
  - `High`: returns results with 0.01C per bit.  Default on some "high
    resolution" hardware.  Limits range, only recommended for Pt100
    sensors.  Automatically changes the default value of `scale`.
- `chXWires`: Number of sensor wires.  Please read Beckhoff's
  documentation for your device, as no device supports all of these
  without additional hardware.  In general, additional sensor wires
  reduce errors with long cable runs.
  - `2`: used for 2-wire sensors.
  - `3`: used for 3-wire sensors.
  - `4`: used for 4-wire sensors.
  
All of these settings are case insensitive.

At the moment, the driver does not attempt to read current values from
the hardware.  This may mean that results are incorrect on some
high-precision (`-0010`, `-0020`, and `-0030` device variants, mostly)
devices.  If you have such hardware and see this problem, either file
a bug (above) or explicitly set `chXResolution` to `High` and verify
that results are accurate.

There are minor differences between pin names for temperature devices
vs generic analog inputs.  Instead of `ain`, the pin names are
prefixed with `temp`, and the temperature value is in `temperature`
instead of `val`.  Example:

```
     8  bit   OUT         FALSE  lcec.0.D16.temp-0-error
     8  bit   OUT         FALSE  lcec.0.D16.temp-0-overrange
     8  s32   OUT          2260  lcec.0.D16.temp-0-raw
     8  float I/O          0.01  lcec.0.D16.temp-0-scale
     8  float OUT          22.6  lcec.0.D16.temp-0-temperature
     8  bit   OUT         FALSE  lcec.0.D16.temp-0-underrange
     8  bit   OUT         FALSE  lcec.0.D16.temp-1-error
```

Selecting sensor type `Ohm/16` or `Ohm/64`, or `High` resolution will
result in the `scale` pin's default changing so that the reported
results are correct for the sensor type chosen.

The author has a EL3204 in his test lab with 4 devices connected:

- Channel 0: Pt100, set to high resolution.
- Channel 1: Ohm/16, with a 1k Ohm resistor.
- Channel 2: Pt1000, set to normal resolution.
- Channel 4: Ohm/64, with a 1k Ohm resistor.

Sample values:

```
     8  float OUT         20.84  lcec.0.D16.temp-0-temperature
     8  float OUT       987.125  lcec.0.D16.temp-1-temperature
     8  float OUT          19.7  lcec.0.D16.temp-2-temperature
     8  float OUT      995.0469  lcec.0.D16.temp-3-temperature
```

Notice that ranges are automatically adjusted for `High` resolution
(channel 0) or `Ohm/` sensor types (channels 1 and 3) so that the
results are correct and do not need additional scaling.

Also, notice that `Ohm` sensor types do not change the pin name.  It's
still `-temperature`, even though the value is now in Ohms.
