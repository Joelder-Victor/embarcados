import { Image, Text, View } from "react-native";
import Auth from "../components/Auth";
import { Separador } from "../components/Separador";

function LoginScreen({ navigation }: any) {
  return (
    <View
      style={{
        flex: 1,
        alignItems: "center",
        justifyContent: "center",
        backgroundColor: "#ffffff",
      }}
    >
      <Image
        style={{ width: 250, height: 250 }}
        source={require("../assets/LOGO.png")}
      />
      <Separador size={128} vertical />
      <Auth navigation={navigation} />
    </View>
  );
}

export default LoginScreen;
