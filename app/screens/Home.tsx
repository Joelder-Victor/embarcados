import React, { useCallback, useEffect, useState } from "react";
import {
    FlatList,
  SafeAreaView,
  StyleSheet,
  TouchableOpacity,
  View,
} from "react-native";
import DeviceModal from "../screens/DeviceConnectionModal";
import { PulseIndicator } from "../components/PulseIndicator";
import useBLE from "../components/useBLE";
import { Avatar, Button, Card, Text } from "react-native-paper";
import { GoogleSignin } from "@react-native-google-signin/google-signin";
import { RouteProp, useRoute } from "@react-navigation/native";
import { Separador } from "../components/Separador";

type IUser = {
  Login: {
    user: any
  };
};
const HomeScreen = ({navigation}) => {
    const route = useRoute<RouteProp<IUser, 'Login'>>();
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

    const LeftContent = props => <Avatar.Image {...props} source={{ uri: route?.params.user.user.photo }}/>

    useEffect(() => {
        scanForDevices;
    }, []);

    return (
        <SafeAreaView style={styles.container}>
        <View style={styles.cardWrapper}>
            <Card>
                <Card.Title title={route?.params.user.user.name} subtitle={route?.params.user.user.email} left={LeftContent} />
                {/* <Card.Content>
                <Avatar.Image size={64} source={{ uri: route?.params.user.user.photo }}/>
                <Text variant="titleLarge">Card title</Text>
                <Text variant="bodyMedium">Card content</Text>
                </Card.Content> */}
            </Card>
        </View>
        <View style={styles.heartRateTitleWrapper}>
            <Text style={styles.heartRateTitleText}>
                Salas disponíveis
            </Text>

            <SafeAreaView style={modalStyle.modalTitle}>
                {/* <Text style={modalStyle.modalTitleText}>
                Tap on a device to connect
                </Text> */}
                <FlatList
                contentContainerStyle={modalStyle.modalFlatlistContiner}
                data={allDevices}
                // renderItem={renderDeviceModalListItem}
                />
            </SafeAreaView>
        </View>
        <TouchableOpacity
            onPress={connectedDevice ? disconnectFromDevice : openModal}
            style={styles.ctaButton}
        >
            <Text style={styles.ctaButtonText}>
            {connectedDevice ? "Disconnect" : "Connect"}
            </Text>
        </TouchableOpacity>
        <TouchableOpacity
            onPress={async () => {
                    try {
                        await GoogleSignin.signOut();
                        navigation.navigate("Login");
                        } catch (error) {
                        console.error(error);
                    }}}
            style={styles.outButton}
        >
            <Text style={styles.outButtonText}>
            {"Sair"}
            </Text>
        </TouchableOpacity>
        
        {/* <DeviceModal
            closeModal={hideModal}
            visible={isModalVisible}
            connectToPeripheral={connectToDevice}
            devices={allDevices}
        /> */}
        </SafeAreaView>
    );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#f2f2f2",
  },
  cardWrapper: {
    flex: 1,
    marginTop: 60,
    marginHorizontal: 20,
  },
  heartRateTitleWrapper: {
    flex: 3,
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
  outButton: {
    backgroundColor: "#4285F4",
    justifyContent: "center",
    alignItems: "center",
    height: 50,
    marginHorizontal: 20,
    marginBottom: 5,
    borderRadius: 8,
  },
  outButtonText: {
    fontSize: 18,
    fontWeight: "bold",
    color: "white",
  },
});

const modalStyle = StyleSheet.create({
  modalContainer: {
    flex: 1,
    backgroundColor: "#f2f2f2",
  },
  modalFlatlistContiner: {
    flex: 1,
    justifyContent: "center",
  },
  modalCellOutline: {
    borderWidth: 1,
    borderColor: "black",
    alignItems: "center",
    marginHorizontal: 20,
    paddingVertical: 15,
    borderRadius: 8,
  },
  modalTitle: {
    flex: 1,
    backgroundColor: "#f2f2f2",
  },
  modalTitleText: {
    marginTop: 40,
    fontSize: 30,
    fontWeight: "bold",
    marginHorizontal: 20,
    textAlign: "center",
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

export default HomeScreen;

// import { View , Text} from "react-native";

// function HomeScreen() {
//   return (
//     <View style={{ flex: 1, alignItems: 'center', justifyContent: 'center' }}>
//       <Text>Home Screen</Text>
//     </View>
//   );
// }

// export default HomeScreen;