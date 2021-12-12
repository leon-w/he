import Card from "@mui/material/Card";
import CardContent from "@mui/material/CardContent";
import Table from "@mui/material/Table";
import TableBody from "@mui/material/TableBody";
import TableCell from "@mui/material/TableCell";
import TableHead from "@mui/material/TableHead";
import TableRow from "@mui/material/TableRow";
import Typography from "@mui/material/Typography";
import * as React from "react";

class DatabaseTable extends React.Component {
    render() {
        const columns = Object.keys(this.props.data ?? {});
        const row_indexes = new Array(this.props.data?.[columns[0]]?.length).fill().map((_, i) => i);

        return (
            <Card variant="outlined" sx={{ mb: 4 * !this.props.last }}>
                <CardContent>
                    <Typography component="h2" variant="h6" color="primary" gutterBottom>
                        Table {this.props.name}
                    </Typography>
                    <Table size="small">
                        <TableHead>
                            <TableRow>
                                <TableCell align="right">ID</TableCell>
                                {columns.map((c, i) => (
                                    <TableCell key={i}>{c}</TableCell>
                                ))}
                            </TableRow>
                        </TableHead>
                        <TableBody>
                            {row_indexes.map(row => (
                                <TableRow key={row}>
                                    <TableCell align="right">{row}</TableCell>
                                    {columns.map((c, i) => (
                                        <TableCell sx={{ fontFamily: "monospace" }} key={i}>
                                            {this.props.data[c][row]}
                                        </TableCell>
                                    ))}
                                </TableRow>
                            ))}
                        </TableBody>
                    </Table>
                </CardContent>
            </Card>
        );
    }
}

export default DatabaseTable;
