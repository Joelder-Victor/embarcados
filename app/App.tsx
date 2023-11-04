import React, { useEffect, useState } from "react";
import { NavigationContainer } from "@react-navigation/native";
import { createNativeStackNavigator } from "@react-navigation/native-stack";
import { ActivityIndicator, PaperProvider } from "react-native-paper";
import LoginScreen from "./screens/Login";
import HomeScreen from "./screens/Home";
import { GoogleSignin } from "@react-native-google-signin/google-signin";
import { View } from "react-native";

const Stack = createNativeStackNavigator();

function App() {
  const [isSigned, setIsSigned] = useState<boolean>(null);

  const isSignedIn = async () => {
    const isSignedIn = await GoogleSignin.isSignedIn();
    setIsSigned(isSignedIn);
  };

  useEffect(() => {
    isSignedIn();
  }, []);
  if (isSigned == null) {
    return (
      <View style={{ flex: 1, justifyContent: "center", alignItems: "center" }}>
        <ActivityIndicator animating={true} color={"black"} size={64} />
      </View>
    );
  } else {
    return (
      <PaperProvider>
        <NavigationContainer>
          {isSigned ? (
            <Stack.Navigator initialRouteName="Home">
              <Stack.Screen
                name="Home"
                component={HomeScreen}
                options={{ headerShown: false, gestureEnabled: false }}
              />
              <Stack.Screen
                name="Login"
                component={LoginScreen}
                options={{ headerShown: false, gestureEnabled: false }}
              />
            </Stack.Navigator>
          ) : (
            <Stack.Navigator initialRouteName="Login">
              <Stack.Screen
                name="Login"
                component={LoginScreen}
                options={{ headerShown: false, gestureEnabled: false }}
              />
              <Stack.Screen
                name="Home"
                component={HomeScreen}
                options={{ headerShown: false, gestureEnabled: false }}
              />
            </Stack.Navigator>
          )}
        </NavigationContainer>
      </PaperProvider>
    );
  }
}

export default App;
