package settings

import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.wrapContentSize
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material.Button
import androidx.compose.material.MaterialTheme
import androidx.compose.material.Text
import androidx.compose.material.TextField
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Settings
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.vector.rememberVectorPainter
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.unit.dp
import cafe.adriel.voyager.koin.getScreenModel
import cafe.adriel.voyager.navigator.tab.Tab
import cafe.adriel.voyager.navigator.tab.TabOptions
import org.jetbrains.compose.resources.ExperimentalResourceApi
import org.jetbrains.compose.resources.getString
import org.jetbrains.compose.resources.stringResource
import shutters.composeapp.generated.resources.Res
import shutters.composeapp.generated.resources.apply
import shutters.composeapp.generated.resources.host
import shutters.composeapp.generated.resources.port
import shutters.composeapp.generated.resources.settings


@OptIn(ExperimentalResourceApi::class)
object SettingsTab: Tab {
    override val options: TabOptions
        @Composable
        get() {
            val icon = rememberVectorPainter(Icons.Default.Settings)
            val title = stringResource(Res.string.settings)

            return remember {
                TabOptions(
                    index = 0u,
                    title = title,
                    icon = icon
                )
            }
        }

    @Composable
    override fun Content() {
        val model = getScreenModel<SettingsScreenModel>()
        val state by model.state.collectAsState()

        LaunchedEffect(key1 = true) {
            model reduce(ViewCreated)
        }
        MaterialTheme {
            Box(modifier = Modifier.padding(20.dp).fillMaxSize()) {
                Column(modifier = Modifier.wrapContentSize()) {
                    TextField(
                        modifier = Modifier.fillMaxWidth().padding(top = 20.dp),
                        value = state.host,
                        onValueChange = { model reduce HostChanged(it) },
                        label = { Text(stringResource(Res.string.host)) }
                    )
                    TextField(
                        modifier = Modifier.fillMaxWidth().padding(top = 20.dp),
                        value = state.port.toString(),
                        keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),
                        onValueChange = { model reduce PortChanged(it) },
                        label = { Text(stringResource(Res.string.port)) }
                    )
                }
                Button(modifier = Modifier.align(Alignment.BottomStart).fillMaxWidth(),
                        enabled = state.isApplyButtonEnabled,
                        onClick = { model reduce ApplyClicked }) {
                    Text(stringResource(Res.string.apply))
                }
            }
        }
    }
}