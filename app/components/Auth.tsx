import {
  GoogleSignin,
  GoogleSigninButton,
  statusCodes,
} from '@react-native-google-signin/google-signin';

export default function({ navigation }) {
     GoogleSignin.configure({
        scopes: ['https://www.googleapis.com/auth/drive.readonly'], // what API you want to access on behalf of the user, default is email and profile
        webClientId: '959788450263-0i9b4lahad2qhov09k68h8caea8cl411.apps.googleusercontent.com', // client ID of type WEB for your server (needed to verify user ID and offline access)
    });

    return(
        <GoogleSigninButton
            size={GoogleSigninButton.Size.Wide}
            color={GoogleSigninButton.Color.Dark}
            onPress={async () => {
                try {
                    await GoogleSignin.hasPlayServices();
                    const userInfo = await GoogleSignin.signIn();
                    navigation.navigate("Home", {user: userInfo});
                } catch (error: any) {
                    if (error.code === statusCodes.SIGN_IN_CANCELLED) {
                    // user cancelled the login flow
                    } else if (error.code === statusCodes.IN_PROGRESS) {
                    // operation (e.g. sign in) is in progress already
                    } else if (error.code === statusCodes.PLAY_SERVICES_NOT_AVAILABLE) {
                    // play services not available or outdated
                    } else {
                    // some other error happened
                    }
                }
            }}
        />
    )
}