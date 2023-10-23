import React, { useState } from "react";
import {
  SafeAreaView,
  StyleSheet,
  Text,
  TouchableOpacity,
  View,
} from "react-native";
import DeviceModal from "./DeviceConnectionModal";
import { PulseIndicator } from "./PulseIndicator";
import useBLE from "./useBLE";
import Auth from "./Auth";

const App = () => {
  const {
    requestPermissions,
    scanForPeripherals,
    allDevices,
    connectToDevice,
    connectedDevice,
    // heartRate,
    disconnectFromDevice,
  } = useBLE();
  const [isModalVisible, setIsModalVisible] = useState<boolean>(false);

  const scanForDevices = async () => {
    const isPermissionsEnabled = await requestPermissions();
    if (isPermissionsEnabled) {
      scanForPeripherals();
    }
  };

  const hideModal = () => {
    setIsModalVisible(false);
  };

  const openModal = async () => {
    scanForDevices();
    setIsModalVisible(true);
  };

  return (
    <SafeAreaView style={styles.container}>
      <View style={styles.heartRateTitleWrapper}>
        {connectedDevice ? (
          <>
            <PulseIndicator />
            <Text style={styles.heartRateTitleText}>Your Heart Rate Is:</Text>
            <Text style={styles.heartRateText}>bpm</Text>
          </>
        ) : (
          <Text style={styles.heartRateTitleText}>
            Please Connect to a Heart Rate Monitor
          </Text>
        )}  
      </View>
      <Auth/>
       <TouchableOpacity
        onPress={connectedDevice ? disconnectFromDevice : openModal}
        style={styles.ctaButton}
      >
        <Text style={styles.ctaButtonText}>
          {connectedDevice ? "Disconnect" : "Connect"}
        </Text>
      </TouchableOpacity>
      <DeviceModal
        closeModal={hideModal}
        visible={isModalVisible}
        connectToPeripheral={connectToDevice}
        devices={allDevices}
      />
    </SafeAreaView>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#f2f2f2",
  },
  heartRateTitleWrapper: {
    flex: 1,
    justifyContent: "center",
    alignItems: "center",
  },
  heartRateTitleText: {
    fontSize: 30,
    fontWeight: "bold",
    textAlign: "center",
    marginHorizontal: 20,
    color: "black",
  },
  heartRateText: {
    fontSize: 25,
    marginTop: 15,
  },
  ctaButton: {
    backgroundColor: "#FF6060",
    justifyContent: "center",
    alignItems: "center",
    height: 50,
    marginHorizontal: 20,
    marginBottom: 5,
    borderRadius: 8,
  },
  ctaButtonText: {
    fontSize: 18,
    fontWeight: "bold",
    color: "white",
  },
});

export default App;

// import { StatusBar } from 'expo-status-bar';
// import { PermissionsAndroid, Platform, StyleSheet, Text, View } from 'react-native';
// import { BleManager, Device } from 'react-native-ble-plx';
// import {useState, useEffect, useRef} from 'react';
// import * as ExpoDevice from "expo-device";



// const manager = new BleManager();

// export default function App() {
//   const [device, setDevice] = useState(null);
//   const [connectionStatus, setConnectionStatus] = useState("Searching for device");
//   const deviceRef = useRef(null);

//   const requestAndroid31Permissions = async () => {
//     const bluetoothScanPermission = await PermissionsAndroid.request(
//       PermissionsAndroid.PERMISSIONS.BLUETOOTH_SCAN,
//       {
//         title: "Location Permission",
//         message: "Bluetooth Low Energy requires Location",
//         buttonPositive: "OK",
//       }
//     );
//     const bluetoothConnectPermission = await PermissionsAndroid.request(
//       PermissionsAndroid.PERMISSIONS.BLUETOOTH_CONNECT,
//       {
//         title: "Location Permission",
//         message: "Bluetooth Low Energy requires Location",
//         buttonPositive: "OK",
//       }
//     );
//     const fineLocationPermission = await PermissionsAndroid.request(
//       PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
//       {
//         title: "Location Permission",
//         message: "Bluetooth Low Energy requires Location",
//         buttonPositive: "OK",
//       }
//     );
//     console.log(bluetoothScanPermission)
//     console.log(bluetoothConnectPermission)
//     console.log(fineLocationPermission)
//     return (
//       bluetoothScanPermission === "granted" &&
//       bluetoothConnectPermission === "granted" &&
//       fineLocationPermission === "granted"
//     );
//   };

//   const requestPermissions = async () => {
//     if (Platform.OS === "android") {
//       if ((ExpoDevice.platformApiLevel ?? -1) < 31) {
//         const granted = await PermissionsAndroid.request(
//           PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
//           {
//             title: "Location Permission",
//             message: "Bluetooth Low Energy requires Location",
//             buttonPositive: "OK",
//           }
//         );
//         return granted === PermissionsAndroid.RESULTS.GRANTED;
//       } else {
//         const isAndroid31PermissionsGranted =
//           await requestAndroid31Permissions();

//         return isAndroid31PermissionsGranted;
//       }
//     } else {
//       return true;
//     }
//   };

//   const searchAndConnectToDevice = () => {
//     console.log('aa?')
//     manager.startDeviceScan(null, null, (error, device) => {
//       console.log(device)
//     });
//   }

//   useEffect(() => {
//     const requestPermissionsAsync = async () => {
//       const isPermissionsEnabled = await requestPermissions();
//       if (isPermissionsEnabled) {
//         searchAndConnectToDevice();
//       }
//     };
//     requestPermissionsAsync();
//   }, []);

//   return (
//     <View style={styles.container}>
//       <Text>Open up App.tsx to start working on your app!</Text>
//       <Text>{connectionStatus}</Text>
//       <StatusBar style="auto" />
//     </View>
//   );
// }

// const styles = StyleSheet.create({
//   container: {
//     flex: 1,
//     backgroundColor: '#fff',
//     alignItems: 'center',
//     justifyContent: 'center',
//   },
// });
