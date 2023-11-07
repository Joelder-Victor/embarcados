/* eslint-disable no-bitwise */
import { useCallback, useMemo, useState } from "react";
import { PermissionsAndroid, Platform } from "react-native";
import {
  Base64,
  BleError,
  BleManager,
  Characteristic,
  Device,
} from "react-native-ble-plx";

import * as ExpoDevice from "expo-device";

import { Buffer } from "buffer";

// import base64 from "react-native-base64";

const UUID_CHARACTERISTIC = "00002a01-0000-1000-8000-00805f9b34fb";
const UUID_SERVICE = "00001802-0000-1000-8000-00805f9b34fb";

interface BluetoothLowEnergyApi {
  requestPermissions(): Promise<boolean>;
  scanForPeripherals(): void;
  connectToDevice: (deviceId: Device) => Promise<void>;
  disconnectFromDevice: () => void;
  connectedDevice: Device | null;
  allDevices: Device[];
  sendKey: (key: string) => Promise<void>;
  statusKey: boolean | null;
//   heartRate: number;
}

const bleManager = new BleManager();

function useBLE(): BluetoothLowEnergyApi {
  const [allDevices, setAllDevices] = useState<Device[]>([]);
  const [connectedDevice, setConnectedDevice] = useState<Device | null>(null);
  const [statusKey, setStatusKey] = useState<boolean>(null);
//   const [heartRate, setHeartRate] = useState<number>(0);

  const requestAndroid31Permissions = async () => {
    const bluetoothScanPermission = await PermissionsAndroid.request(
      PermissionsAndroid.PERMISSIONS.BLUETOOTH_SCAN,
      {
        title: "Location Permission",
        message: "Bluetooth Low Energy requires Location",
        buttonPositive: "OK",
      }
    );
    const bluetoothConnectPermission = await PermissionsAndroid.request(
      PermissionsAndroid.PERMISSIONS.BLUETOOTH_CONNECT,
      {
        title: "Location Permission",
        message: "Bluetooth Low Energy requires Location",
        buttonPositive: "OK",
      }
    );
    const fineLocationPermission = await PermissionsAndroid.request(
      PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
      {
        title: "Location Permission",
        message: "Bluetooth Low Energy requires Location",
        buttonPositive: "OK",
      }
    );
    return (
      bluetoothScanPermission === "granted" &&
      bluetoothConnectPermission === "granted" &&
      fineLocationPermission === "granted"
    );
  };

  const requestPermissions = async () => {
    if (Platform.OS === "android") {
      if ((ExpoDevice.platformApiLevel ?? -1) < 31) {
        const granted = await PermissionsAndroid.request(
          PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
          {
            title: "Location Permission",
            message: "Bluetooth Low Energy requires Location",
            buttonPositive: "OK",
          }
        );
        return granted === PermissionsAndroid.RESULTS.GRANTED;
      } else {
        const isAndroid31PermissionsGranted =
          await requestAndroid31Permissions();

        return isAndroid31PermissionsGranted;
      }
    } else {
      return true;
    }
  };

  const isDuplicteDevice = (devices: Device[], nextDevice: Device) =>
    devices.findIndex((device) => nextDevice.id === device.id) > -1;
  
  const scanForPeripherals = () =>
    bleManager.startDeviceScan(null, null, (error, device) => {
      if (error) {
        console.log(error);
      }
      if (device && device.name?.includes("SALA")) {
        setAllDevices((prevState: Device[]) => {
          if (!isDuplicteDevice(prevState, device)) {
            return [...prevState, device];
          }
          return prevState;
        });
      }
    });

  const connectToDevice = async (device: Device) => {
    try {
      const deviceConnection = await bleManager.connectToDevice(device.id);
      setConnectedDevice(deviceConnection);
      await bleManager.discoverAllServicesAndCharacteristicsForDevice(device.id);
      const services = await deviceConnection.characteristicsForService(UUID_SERVICE);
      // console.log(deviceConnection.services())
      console.log(services);      
      bleManager.stopDeviceScan();
    } catch (e) {
      console.log("FAILED TO CONNECT", e);
    }
  };

  const disconnectFromDevice = () => {
    if (connectedDevice) {
      bleManager.cancelDeviceConnection(connectedDevice.id);
      setConnectedDevice(null);
      //setHeartRate(0);
    }
  };


  const encodeStringToBase64 = (value: string) => {
    console.log(Buffer.from(value).toString("base64"));
    return Buffer.from(value).toString("base64");
  };

 const sendKey = async (key: string) => {
    try {
      await connectedDevice.writeCharacteristicWithoutResponseForService(
        UUID_SERVICE,
        UUID_CHARACTERISTIC,
        encodeStringToBase64(key)
      );
      setStatusKey(true);
      console.log('enviado')
    } catch (e) {
      console.log("FAILED TO SEND", e);
    }
  };
  // const sendKey = useCallback(
  // async (key: string) => {
  //   try {
  //     console.log('entra')
  //     await connectedDevice.writeCharacteristicWithoutResponseForService(
  //       UUID_SERVICE,
  //       UUID_CHARACTERISTIC,
  //       encodeStringToBase64(key)
  //     );
  //     setStatusKey(true);
  //   } catch (error) {
  //     throw new Error(JSON.stringify(error));
  //   }
  // },
  // [connectedDevice]
// );

  return {
    scanForPeripherals,
    requestPermissions,
    connectToDevice,
    allDevices,
    connectedDevice,
    disconnectFromDevice,
    sendKey,
    statusKey,
    // heartRate,
  };
}

export default useBLE;