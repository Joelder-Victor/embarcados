import { Text, View } from "react-native";
import Auth from "../components/Auth";

function LoginScreen({ navigation }: any) {
  return (
    <View style={{ flex: 1, alignItems: "center", justifyContent: "center" }}>
      <Text>Login Screen</Text>
      <Auth navigation={navigation} />
    </View>
  );
}

export default LoginScreen;
