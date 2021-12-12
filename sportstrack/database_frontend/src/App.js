import DnsIcon from "@mui/icons-material/Dns";
import RefreshIcon from "@mui/icons-material/Refresh";
import Alert from "@mui/material/Alert";
import AppBar from "@mui/material/AppBar";
import Container from "@mui/material/Container";
import IconButton from "@mui/material/IconButton";
import Toolbar from "@mui/material/Toolbar";
import Typography from "@mui/material/Typography";
import * as React from "react";
import DatabaseTable from "./DatabaseTable";

class App extends React.Component {
    state = {
        databases: [],
        database_names: [],
        error: null,
    };

    componentWillMount() {
        this.updateData();
    }

    updateData() {
        fetch("http://localhost:8080/api/get_database_dump")
            .then(x => x.json())
            .then(data => {
                const repetitions_db = data.database.repetitions;
                const repetitions_sum_db = { CIPHERTEXT: [data.database.repetitions_sum] };
                this.setState({
                    databases: [repetitions_db, repetitions_sum_db],
                    database_names: ["REPETITIONS", "REPETITIONS_SUM"],
                    error: null,
                });
            })
            .catch(err => {
                this.setState({
                    error: err,
                    databases: [],
                    database_names: [],
                });
            });
    }

    render() {
        return (
            <>
                <AppBar position="relative">
                    <Toolbar>
                        <DnsIcon sx={{ mr: 2 }} />
                        <Typography variant="h6" color="inherit" noWrap sx={{ flexGrow: 1 }}>
                            SportsTrack Database Viewer
                        </Typography>
                        <IconButton color="inherit" aria-label="open drawer" onClick={() => this.updateData()}>
                            <RefreshIcon />
                        </IconButton>
                    </Toolbar>
                </AppBar>
                <main>
                    <Container sx={{ py: 8 }} maxWidth="md">
                        {this.state.error && (
                            <Alert severity="error" onClose={() => this.setState({ error: null })}>
                                {this.state.error.toString()}
                            </Alert>
                        )}
                        {this.state.databases.map((db, i) => (
                            <DatabaseTable
                                key={i}
                                name={this.state.database_names[i]}
                                data={db}
                                last={this.state.databases.length - 1 === i}
                            />
                        ))}
                    </Container>
                </main>
            </>
        );
    }
}

export default App;
