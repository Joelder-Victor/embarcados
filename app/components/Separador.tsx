import React from 'react';
import { View, StyleProp, ViewStyle, DimensionValue } from 'react-native';

interface SeparatorProps {
    size?: number;
    vertical?: boolean;
}

export function Separador(props: SeparatorProps) {
    const styles = {
        width: props.vertical ? '100%' : props.size || 8,
        height: props.vertical ? props.size || 8 : '100%',
    } as StyleProp<ViewStyle & { height: number | string }>; // Change DimensionValue to number | string
    return <View style={styles} />;
}