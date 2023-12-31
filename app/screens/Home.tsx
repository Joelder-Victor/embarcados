import React, { FC, useCallback, useEffect, useState } from "react";
import {
  FlatList,
  ListRenderItemInfo,
  SafeAreaView,
  StyleSheet,
  TouchableOpacity,
  View,
} from "react-native";
import useBLE from "../components/useBLE";
import {
  ActivityIndicator,
  Avatar,
  Button,
  Card,
  Dialog,
  Portal,
  Text,
} from "react-native-paper";
import { GoogleSignin, User } from "@react-native-google-signin/google-signin";
import { RouteProp, useRoute } from "@react-navigation/native";
import { Device } from "react-native-ble-plx";

type IUser = {
  Login: {
    user: any;
  };
};

type DeviceModalListItemProps = {
  item: ListRenderItemInfo<Device>;
  connectToPeripheral: (device: Device) => void;
};

const HomeScreen = ({ navigation }: any) => {
  const route = useRoute<RouteProp<IUser, "Login">>();
  const {
    requestPermissions,
    scanForPeripherals,
    allDevices,
    connectToDevice,
    connectedDevice,
    disconnectFromDevice,
    sendKey,
    statusKey,
  } = useBLE();
  const [user, setUser] = useState<User | null>();
  const [visible, setVisible] = React.useState(false);
  const [key, setKey] = useState<string>("");

  const scanForDevices = async () => {
    const isPermissionsEnabled = await requestPermissions();
    if (isPermissionsEnabled) {
      scanForPeripherals();
    }
  };

  const showDialog = () => setVisible(true);

  const hideDialog = () => setVisible(false);

  const LeftContent = (props: any) => (
    <Avatar.Image {...props} source={{ uri: user?.user.photo }} />
  );

  const getCurrentUser = async () => {
    const currentUser = await GoogleSignin.getCurrentUser();
    setUser(currentUser);
  };

  const getUserKey = async () => {
    const userKey = await fetch("https://laser-1.onrender.com/app/get_key", {
      method: "POST",
      headers: {
        accept: "application/json",
        "Content-Type": "application/json",
      },
      body: JSON.stringify({
        email: user?.user.email,
      }),
    })
      .then((response) => response.json())
      .then((data) => {
        setKey(data.key); // Faça o que deseja com os dados de resposta aqui
      })
      .catch((error) => {
        console.error("Erro:", error);
      });
  };

  const DeviceModalListItem: FC<DeviceModalListItemProps> = (props) => {
    const { item, connectToPeripheral } = props;

    const connectAndCloseModal = useCallback(async () => {
      connectToPeripheral(item.item);
    }, [connectToPeripheral, item.item]);

    return (
      <TouchableOpacity
        onPress={connectAndCloseModal}
        style={modalStyle.ctaButton}
      >
        <Text style={modalStyle.ctaButtonText}>{item.item.name}</Text>
      </TouchableOpacity>
    );
  };

  const renderDeviceModalListItem = useCallback(
    (item: ListRenderItemInfo<Device>) => {
      return (
        <DeviceModalListItem
          item={item}
          connectToPeripheral={connectToDevice}
        />
      );
    },
    [connectToDevice]
  );

  useEffect(() => {
    if (route.params) {
      setUser(route.params.user);
    } else {
      getCurrentUser();
    }
    GoogleSignin.configure({});
    getUserKey();
    scanForDevices();
  }, []);

  useEffect(() => {
    if (connectedDevice) {
      showDialog();
    }
  }, [connectedDevice]);

  const disconectAndClose = useCallback(async () => {
    disconnectFromDevice();
    console.log("desconectando...");
    hideDialog();
  }, [connectedDevice]);

  const sendKeytoESP = useCallback(async () => {
    sendKey(key);
    console.log(statusKey);
    if (statusKey) {
      disconectAndClose();
    }
  }, [key, statusKey]);

  if (user == null) {
    return (
      <View style={{ flex: 1, justifyContent: "center", alignItems: "center" }}>
        <ActivityIndicator animating={true} color={"black"} size={64} />
      </View>
    );
  } else {
    return (
      <SafeAreaView style={styles.container}>
        <View style={styles.cardWrapper}>
          <Card>
            <Card.Title
              title={user?.user.name}
              subtitle={user?.user.email}
              left={LeftContent}
            />
          </Card>
        </View>
        <View style={styles.heartRateTitleWrapper}>
          <Text style={styles.heartRateTitleText}>Salas disponíveis</Text>
          <FlatList
            contentContainerStyle={modalStyle.modalFlatlistContiner}
            data={allDevices}
            renderItem={renderDeviceModalListItem}
          />
        </View>
        <TouchableOpacity
          onPress={
            connectedDevice ? disconnectFromDevice : disconnectFromDevice
          }
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
            }
          }}
          style={styles.outButton}
        >
          <Text style={styles.outButtonText}>{"Sair"}</Text>
        </TouchableOpacity>
        <View>
          <Portal>
            <Dialog visible={visible}>
              <Dialog.Title>{connectedDevice?.name}</Dialog.Title>
              <Dialog.Content>
                <Text variant="bodyMedium">
                  Conectado na porta com sucesso.
                </Text>
              </Dialog.Content>
              <Dialog.Actions>
                <Button onPress={sendKeytoESP}>Abrir</Button>
                <Button onPress={disconectAndClose}>Desconectar</Button>
              </Dialog.Actions>
            </Dialog>
          </Portal>
        </View>
      </SafeAreaView>
    );
  }
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#ffffff",
  },
  cardWrapper: {
    flex: 1,
    marginTop: 60,
    marginHorizontal: 20,
    backgroundColor: "#ffffff",
  },
  heartRateTitleWrapper: {
    flex: 5,
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
    backgroundColor: "#049cfb",
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
    backgroundColor: "#049cfb",
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
